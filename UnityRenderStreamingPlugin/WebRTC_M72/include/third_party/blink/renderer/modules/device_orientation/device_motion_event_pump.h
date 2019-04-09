// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_DEVICE_ORIENTATION_DEVICE_MOTION_EVENT_PUMP_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_DEVICE_ORIENTATION_DEVICE_MOTION_EVENT_PUMP_H_

#include "base/macros.h"
#include "third_party/blink/renderer/modules/device_orientation/device_sensor_event_pump.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class DeviceMotionData;
class PlatformEventController;

class MODULES_EXPORT DeviceMotionEventPump
    : public GarbageCollectedFinalized<DeviceMotionEventPump>,
      public DeviceSensorEventPump {
 public:
  explicit DeviceMotionEventPump(scoped_refptr<base::SingleThreadTaskRunner>);
  ~DeviceMotionEventPump() override;

  void SetController(PlatformEventController*);
  void RemoveController();

  // Note that the returned object is owned by this class.
  DeviceMotionData* LatestDeviceMotionData();

  void Trace(blink::Visitor*);

  // DeviceSensorEventPump:
  void SendStartMessage(LocalFrame* frame) override;
  void SendStopMessage() override;

 protected:
  // DeviceSensorEventPump:
  void FireEvent(TimerBase*) override;

  SensorEntry accelerometer_;
  SensorEntry linear_acceleration_sensor_;
  SensorEntry gyroscope_;

 private:
  friend class DeviceMotionEventPumpTest;

  void StartListening(LocalFrame*);
  void StopListening();
  void NotifyController();

  // DeviceSensorEventPump:
  bool SensorsReadyOrErrored() const override;

  DeviceMotionData* GetDataFromSharedMemory();

  Member<DeviceMotionData> data_;
  WeakMember<PlatformEventController> controller_;

  DISALLOW_COPY_AND_ASSIGN(DeviceMotionEventPump);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_DEVICE_ORIENTATION_DEVICE_MOTION_EVENT_PUMP_H_
