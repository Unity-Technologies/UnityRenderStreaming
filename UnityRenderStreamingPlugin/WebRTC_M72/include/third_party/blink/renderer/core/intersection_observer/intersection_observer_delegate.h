// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_INTERSECTION_OBSERVER_INTERSECTION_OBSERVER_DELEGATE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_INTERSECTION_OBSERVER_INTERSECTION_OBSERVER_DELEGATE_H_

#include "third_party/blink/renderer/platform/bindings/name_client.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class ExecutionContext;
class IntersectionObserver;
class IntersectionObserverEntry;

class IntersectionObserverDelegate
    : public GarbageCollectedFinalized<IntersectionObserverDelegate>,
      public NameClient {
 public:
  virtual ~IntersectionObserverDelegate() = default;
  virtual void Deliver(const HeapVector<Member<IntersectionObserverEntry>>&,
                       IntersectionObserver&) = 0;
  virtual ExecutionContext* GetExecutionContext() const = 0;
  virtual void Trace(blink::Visitor* visitor) {}
  const char* NameInHeapSnapshot() const override {
    return "IntersectionObserverDelegate";
  }
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_INTERSECTION_OBSERVER_INTERSECTION_OBSERVER_DELEGATE_H_
