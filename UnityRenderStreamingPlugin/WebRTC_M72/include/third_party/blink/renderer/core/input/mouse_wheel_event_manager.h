// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_INPUT_MOUSE_WHEEL_EVENT_MANAGER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_INPUT_MOUSE_WHEEL_EVENT_MANAGER_H_

#include "base/macros.h"
#include "third_party/blink/public/platform/web_input_event_result.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/heap/visitor.h"

namespace blink {

class Document;
class LocalFrame;
class LocalFrameView;
class Node;
class WebMouseWheelEvent;

class MouseWheelEventManager final
    : public GarbageCollectedFinalized<MouseWheelEventManager> {
 public:
  explicit MouseWheelEventManager(LocalFrame&);
  void Trace(blink::Visitor*);

  void Clear();

  WebInputEventResult HandleWheelEvent(const WebMouseWheelEvent&);

  void ElementRemoved(Node* target);

 private:
  Node* FindTargetNode(const WebMouseWheelEvent&,
                       const Document*,
                       const LocalFrameView*);

  const Member<LocalFrame> frame_;
  Member<Node> wheel_target_;

  DISALLOW_COPY_AND_ASSIGN(MouseWheelEventManager);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_INPUT_MOUSE_WHEEL_EVENT_MANAGER_H_
