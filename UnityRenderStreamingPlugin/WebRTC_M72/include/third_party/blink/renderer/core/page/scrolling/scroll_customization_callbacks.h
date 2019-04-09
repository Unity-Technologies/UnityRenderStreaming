// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_SCROLLING_SCROLL_CUSTOMIZATION_CALLBACKS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_SCROLLING_SCROLL_CUSTOMIZATION_CALLBACKS_H_

#include "base/macros.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/hash_map.h"

namespace blink {

class Node;
class ScrollStateCallback;

class CORE_EXPORT ScrollCustomizationCallbacks
    : public GarbageCollected<ScrollCustomizationCallbacks> {
 public:
  ScrollCustomizationCallbacks() = default;
  void SetDistributeScroll(Node*, ScrollStateCallback*);
  ScrollStateCallback* GetDistributeScroll(Node*);
  void SetApplyScroll(Node*, ScrollStateCallback*);
  void RemoveApplyScroll(Node*);
  ScrollStateCallback* GetApplyScroll(Node*);
  bool InScrollPhase(Node*) const;
  void SetInScrollPhase(Node*, bool);

  void Trace(blink::Visitor* visitor) {
    visitor->Trace(apply_scroll_callbacks_);
    visitor->Trace(distribute_scroll_callbacks_);
    visitor->Trace(in_scrolling_phase_);
  };

 private:
  using ScrollStateCallbackList =
      HeapHashMap<WeakMember<Node>, Member<ScrollStateCallback>>;
  ScrollStateCallbackList apply_scroll_callbacks_;
  ScrollStateCallbackList distribute_scroll_callbacks_;
  HeapHashMap<WeakMember<Node>, bool> in_scrolling_phase_;

  DISALLOW_COPY_AND_ASSIGN(ScrollCustomizationCallbacks);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_SCROLLING_SCROLL_CUSTOMIZATION_CALLBACKS_H_
