// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EVENTS_VISUAL_VIEWPORT_RESIZE_EVENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EVENTS_VISUAL_VIEWPORT_RESIZE_EVENT_H_

#include "third_party/blink/renderer/core/dom/events/event.h"

namespace blink {

class VisualViewportResizeEvent final : public Event {
 public:
  ~VisualViewportResizeEvent() override;

  static VisualViewportResizeEvent* Create() {
    return new VisualViewportResizeEvent();
  }

  void DoneDispatchingEventAtCurrentTarget() override;

  void Trace(blink::Visitor* visitor) override { Event::Trace(visitor); }

 private:
  VisualViewportResizeEvent();
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_EVENTS_VISUAL_VIEWPORT_RESIZE_EVENT_H_
