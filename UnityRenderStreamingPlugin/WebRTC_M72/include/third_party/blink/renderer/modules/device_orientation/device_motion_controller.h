// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_DEVICE_ORIENTATION_DEVICE_MOTION_CONTROLLER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_DEVICE_ORIENTATION_DEVICE_MOTION_CONTROLLER_H_

#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/frame/device_single_window_event_controller.h"
#include "third_party/blink/renderer/modules/modules_export.h"

namespace blink {

class Event;
class DeviceMotionEventPump;

class MODULES_EXPORT DeviceMotionController final
    : public DeviceSingleWindowEventController,
      public Supplement<Document> {
  USING_GARBAGE_COLLECTED_MIXIN(DeviceMotionController);

 public:
  static const char kSupplementName[];

  explicit DeviceMotionController(Document&);
  ~DeviceMotionController() override;

  static DeviceMotionController& From(Document&);

  // DeviceSingleWindowEventController
  void DidAddEventListener(LocalDOMWindow*,
                           const AtomicString& event_type) override;

  void Trace(blink::Visitor*) override;

 private:
  // Inherited from PlatformEventController.
  void RegisterWithDispatcher() override;
  void UnregisterWithDispatcher() override;
  bool HasLastData() override;

  // Inherited from DeviceSingleWindowEventController.
  Event* LastEvent() const override;
  const AtomicString& EventTypeName() const override;
  bool IsNullEvent(Event*) const override;

  Member<DeviceMotionEventPump> motion_event_pump_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_DEVICE_ORIENTATION_DEVICE_MOTION_CONTROLLER_H_
