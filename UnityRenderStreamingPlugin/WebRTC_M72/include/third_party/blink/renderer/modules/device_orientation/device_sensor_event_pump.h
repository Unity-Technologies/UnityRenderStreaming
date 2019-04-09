// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_DEVICE_ORIENTATION_DEVICE_SENSOR_EVENT_PUMP_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_DEVICE_ORIENTATION_DEVICE_SENSOR_EVENT_PUMP_H_

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "mojo/public/cpp/bindings/binding.h"
#include "services/device/public/cpp/generic_sensor/sensor_reading.h"
#include "services/device/public/cpp/generic_sensor/sensor_reading_shared_buffer_reader.h"
#include "services/device/public/mojom/sensor_provider.mojom-blink.h"
#include "third_party/blink/renderer/platform/timer.h"
#include "third_party/blink/renderer/platform/wtf/functional.h"

namespace blink {

class LocalFrame;

class DeviceSensorEventPump {
 public:
  // Default rate for firing events.
  static constexpr int kDefaultPumpFrequencyHz = 60;
  static constexpr int kDefaultPumpDelayMicroseconds =
      WTF::Time::kMicrosecondsPerSecond / kDefaultPumpFrequencyHz;

  // The pump is a tri-state automaton with allowed transitions as follows:
  // STOPPED -> PENDING_START
  // PENDING_START -> RUNNING
  // PENDING_START -> STOPPED
  // RUNNING -> STOPPED
  enum class PumpState { STOPPED, RUNNING, PENDING_START };

  // The sensor state is an automaton with allowed transitions as follows:
  // NOT_INITIALIZED -> INITIALIZING
  // INITIALIZING -> ACTIVE
  // INITIALIZING -> SHOULD_SUSPEND
  // ACTIVE -> SUSPENDED
  // SHOULD_SUSPEND -> INITIALIZING
  // SHOULD_SUSPEND -> SUSPENDED
  // SUSPENDED -> ACTIVE
  // { INITIALIZING, ACTIVE, SHOULD_SUSPEND, SUSPENDED } -> NOT_INITIALIZED
  enum class SensorState {
    NOT_INITIALIZED,
    INITIALIZING,
    ACTIVE,
    SHOULD_SUSPEND,
    SUSPENDED
  };

  virtual void Start(LocalFrame* frame) {
    DVLOG(2) << "requested start";

    if (state_ != PumpState::STOPPED)
      return;

    DCHECK(!timer_.IsActive());

    state_ = PumpState::PENDING_START;

    DCHECK(!is_observing_);
    is_observing_ = true;

    SendStartMessage(frame);
  }

  virtual void Stop() {
    DVLOG(2) << "requested stop";

    if (state_ == PumpState::STOPPED)
      return;

    DCHECK((state_ == PumpState::PENDING_START && !timer_.IsActive()) ||
           (state_ == PumpState::RUNNING && timer_.IsActive()));

    if (timer_.IsActive())
      timer_.Stop();

    DCHECK(is_observing_);
    is_observing_ = false;

    SendStopMessage();

    state_ = PumpState::STOPPED;
  }

  void HandleSensorProviderError() { sensor_provider_.reset(); }

  void SetSensorProviderForTesting(
      device::mojom::blink::SensorProviderPtr sensor_provider) {
    sensor_provider_ = std::move(sensor_provider);
  }

  PumpState GetPumpStateForTesting() { return state_; }

 protected:
  explicit DeviceSensorEventPump(
      scoped_refptr<base::SingleThreadTaskRunner> task_runner)
      : state_(PumpState::STOPPED),
        timer_(task_runner, this, &DeviceSensorEventPump::FireEvent) {}

  virtual ~DeviceSensorEventPump() { DCHECK(!is_observing_); }

  // This method is expected to send an IPC to the browser process to let it
  // know that it should start observing.
  // It is expected for subclasses to override it.
  virtual void SendStartMessage(LocalFrame*) = 0;

  // This method is expected to send an IPC to the browser process to let it
  // know that it should start observing.
  // It is expected for subclasses to override it.
  virtual void SendStopMessage() = 0;

  // Implementations of DeviceSensorEventPump must call StopIfObserving()
  // from their destructor to shutdown in an orderly manner.
  // (As Stop() calls a virtual method, it cannot be handled by
  // ~DeviceSensorEventPump.)
  void StopIfObserving() {
    if (is_observing_)
      Stop();
  }

  // Even though the TimerBase* parameter is not used, it is required by
  // TaskRunnerTimer class
  virtual void FireEvent(TimerBase*) = 0;

  struct SensorEntry : public device::mojom::blink::SensorClient {
    SensorEntry(DeviceSensorEventPump* pump,
                device::mojom::blink::SensorType sensor_type)
        : event_pump(pump),
          sensor_state(SensorState::NOT_INITIALIZED),
          type(sensor_type),
          client_binding(this) {}

    ~SensorEntry() override {}

    // device::mojom::SensorClient:
    void RaiseError() override { HandleSensorError(); }

    // device::mojom::SensorClient:
    void SensorReadingChanged() override {
      // Since DeviceSensorEventPump::FireEvent is called in a fixed
      // frequency, the |shared_buffer| is read frequently, and
      // Sensor::ConfigureReadingChangeNotifications() is set to false,
      // so this method is not called and doesn't need to be implemented.
      NOTREACHED();
    }

    // Mojo callback for SensorProvider::GetSensor().
    void OnSensorCreated(device::mojom::blink::SensorCreationResult result,
                         device::mojom::blink::SensorInitParamsPtr params) {
      // |sensor_state| can be SensorState::SHOULD_SUSPEND if Stop() is called
      // before OnSensorCreated() is called.
      DCHECK(sensor_state == SensorState::INITIALIZING ||
             sensor_state == SensorState::SHOULD_SUSPEND);

      if (!params) {
        HandleSensorError();
        event_pump->DidStartIfPossible();
        return;
      }
      DCHECK_EQ(device::mojom::SensorCreationResult::SUCCESS, result);

      constexpr size_t kReadBufferSize =
          sizeof(device::SensorReadingSharedBuffer);

      DCHECK_EQ(0u, params->buffer_offset % kReadBufferSize);

      mode = params->mode;
      default_config = std::move(params->default_configuration);

      sensor.Bind(std::move(params->sensor));
      client_binding.Bind(std::move(params->client_request));

      shared_buffer_handle = std::move(params->memory);
      DCHECK(!shared_buffer);
      shared_buffer = shared_buffer_handle->MapAtOffset(kReadBufferSize,
                                                        params->buffer_offset);
      if (!shared_buffer) {
        HandleSensorError();
        event_pump->DidStartIfPossible();
        return;
      }

      const device::SensorReadingSharedBuffer* buffer =
          static_cast<const device::SensorReadingSharedBuffer*>(
              shared_buffer.get());
      shared_buffer_reader.reset(
          new device::SensorReadingSharedBufferReader(buffer));

      default_config->frequency =
          std::min(static_cast<double>(kDefaultPumpFrequencyHz),
                   params->maximum_frequency);

      sensor.set_connection_error_handler(
          WTF::Bind(&SensorEntry::HandleSensorError, WTF::Unretained(this)));
      sensor->ConfigureReadingChangeNotifications(false /* disabled */);
      sensor->AddConfiguration(std::move(default_config),
                               WTF::Bind(&SensorEntry::OnSensorAddConfiguration,
                                         WTF::Unretained(this)));
    }

    // Mojo callback for Sensor::AddConfiguration().
    void OnSensorAddConfiguration(bool success) {
      if (!success)
        HandleSensorError();

      if (sensor_state == SensorState::INITIALIZING) {
        sensor_state = SensorState::ACTIVE;
        event_pump->DidStartIfPossible();
      } else if (sensor_state == SensorState::SHOULD_SUSPEND) {
        sensor->Suspend();
        sensor_state = SensorState::SUSPENDED;
      }
    }

    void HandleSensorError() {
      sensor.reset();
      sensor_state = SensorState::NOT_INITIALIZED;
      shared_buffer_handle.reset();
      shared_buffer.reset();
      client_binding.Close();
    }

    bool SensorReadingCouldBeRead() {
      if (!sensor)
        return false;

      DCHECK(shared_buffer);

      if (!shared_buffer_handle->is_valid() ||
          !shared_buffer_reader->GetReading(&reading)) {
        HandleSensorError();
        return false;
      }

      return true;
    }

    bool ReadyOrErrored() const {
      // When some sensors are not available, the pump still needs to fire
      // events which set the unavailable sensor data fields to null.
      return sensor_state == SensorState::ACTIVE ||
             sensor_state == SensorState::NOT_INITIALIZED;
    }

    void Start(device::mojom::blink::SensorProvider* sensor_provider) {
      if (sensor_state == SensorState::NOT_INITIALIZED) {
        sensor_state = SensorState::INITIALIZING;
        sensor_provider->GetSensor(
            type,
            WTF::Bind(&SensorEntry::OnSensorCreated, WTF::Unretained(this)));
      } else if (sensor_state == SensorState::SUSPENDED) {
        sensor->Resume();
        sensor_state = SensorState::ACTIVE;
        event_pump->DidStartIfPossible();
      } else if (sensor_state == SensorState::SHOULD_SUSPEND) {
        // This can happen when calling Start(), Stop(), Start() in a sequence:
        // After the first Start() call, the sensor state is
        // SensorState::INITIALIZING. Then after the Stop() call, the sensor
        // state is SensorState::SHOULD_SUSPEND, and the next Start() call needs
        // to set the sensor state to be SensorState::INITIALIZING again.
        sensor_state = SensorState::INITIALIZING;
      } else {
        NOTREACHED();
      }
    }

    void Stop() {
      if (sensor) {
        sensor->Suspend();
        sensor_state = SensorState::SUSPENDED;
      } else if (sensor_state == SensorState::INITIALIZING) {
        // When the sensor needs to be suspended, and it is still in the
        // SensorState::INITIALIZING state, the sensor creation is not affected
        // (the SensorEntry::OnSensorCreated() callback will run as usual), but
        // the sensor is marked as SensorState::SHOULD_SUSPEND, and when the
        // sensor is created successfully, it will be suspended and its state
        // will be marked as SensorState::SUSPENDED in the
        // SensorEntry::OnSensorAddConfiguration().
        sensor_state = SensorState::SHOULD_SUSPEND;
      }
    }

    DeviceSensorEventPump* event_pump;
    device::mojom::blink::SensorPtr sensor;
    SensorState sensor_state;
    device::mojom::blink::SensorType type;
    device::mojom::blink::ReportingMode mode;
    device::mojom::blink::SensorConfigurationPtr default_config;
    mojo::ScopedSharedBufferHandle shared_buffer_handle;
    mojo::ScopedSharedBufferMapping shared_buffer;
    std::unique_ptr<device::SensorReadingSharedBufferReader>
        shared_buffer_reader;
    device::SensorReading reading;
    mojo::Binding<device::mojom::blink::SensorClient> client_binding;
  };

  friend struct SensorEntry;

  virtual void DidStartIfPossible() {
    DVLOG(2) << "did start sensor event pump";

    if (state_ != PumpState::PENDING_START)
      return;

    if (!SensorsReadyOrErrored())
      return;

    DCHECK(!timer_.IsActive());

    timer_.StartRepeating(
        WTF::TimeDelta::FromMicroseconds(kDefaultPumpDelayMicroseconds),
        FROM_HERE);
    state_ = PumpState::RUNNING;
  }

  device::mojom::blink::SensorProviderPtr sensor_provider_;

 private:
  virtual bool SensorsReadyOrErrored() const = 0;

  PumpState state_;
  bool is_observing_ = false;
  TaskRunnerTimer<DeviceSensorEventPump> timer_;

  DISALLOW_COPY_AND_ASSIGN(DeviceSensorEventPump);
};
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_DEVICE_ORIENTATION_DEVICE_SENSOR_EVENT_PUMP_H_
