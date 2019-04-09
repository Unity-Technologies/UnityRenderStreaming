/*
 * Copyright 2010, The Android Open Source Project
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_DEVICE_ORIENTATION_DEVICE_ORIENTATION_EVENT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_DEVICE_ORIENTATION_DEVICE_ORIENTATION_EVENT_H_

#include "third_party/blink/renderer/modules/event_modules.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class DeviceOrientationEventInit;
class DeviceOrientationData;

class DeviceOrientationEvent final : public Event {
  DEFINE_WRAPPERTYPEINFO();

 public:
  DeviceOrientationEvent();
  DeviceOrientationEvent(const AtomicString&,
                         const DeviceOrientationEventInit*);
  DeviceOrientationEvent(const AtomicString& event_type,
                         DeviceOrientationData*);
  ~DeviceOrientationEvent() override;

  static DeviceOrientationEvent* Create() {
    return MakeGarbageCollected<DeviceOrientationEvent>();
  }
  static DeviceOrientationEvent* Create(
      const AtomicString& event_type,
      const DeviceOrientationEventInit* initializer) {
    return MakeGarbageCollected<DeviceOrientationEvent>(event_type,
                                                        initializer);
  }
  static DeviceOrientationEvent* Create(const AtomicString& event_type,
                                        DeviceOrientationData* orientation) {
    return MakeGarbageCollected<DeviceOrientationEvent>(event_type,
                                                        orientation);
  }

  DeviceOrientationData* Orientation() const { return orientation_.Get(); }

  double alpha(bool& is_null) const;
  double beta(bool& is_null) const;
  double gamma(bool& is_null) const;
  bool absolute() const;

  const AtomicString& InterfaceName() const override;

  void Trace(blink::Visitor*) override;

 private:
  Member<DeviceOrientationData> orientation_;
};

DEFINE_TYPE_CASTS(DeviceOrientationEvent,
                  Event,
                  event,
                  event->InterfaceName() ==
                      event_interface_names::kDeviceOrientationEvent,
                  event.InterfaceName() ==
                      event_interface_names::kDeviceOrientationEvent);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_DEVICE_ORIENTATION_DEVICE_ORIENTATION_EVENT_H_
