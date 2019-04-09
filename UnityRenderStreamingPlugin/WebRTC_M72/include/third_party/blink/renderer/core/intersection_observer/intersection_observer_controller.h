// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_INTERSECTION_OBSERVER_INTERSECTION_OBSERVER_CONTROLLER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_INTERSECTION_OBSERVER_INTERSECTION_OBSERVER_CONTROLLER_H_

#include "third_party/blink/renderer/core/dom/pausable_object.h"
#include "third_party/blink/renderer/core/intersection_observer/intersection_observer.h"
#include "third_party/blink/renderer/platform/bindings/name_client.h"
#include "third_party/blink/renderer/platform/bindings/trace_wrapper_member.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/hash_set.h"

// Design doc for IntersectionObserver implementation:
//   https://docs.google.com/a/google.com/document/d/1hLK0eyT5_BzyNS4OkjsnoqqFQDYCbKfyBinj94OnLiQ

namespace blink {

class Document;

class IntersectionObserverController
    : public GarbageCollectedFinalized<IntersectionObserverController>,
      public PausableObject,
      public NameClient {
  USING_GARBAGE_COLLECTED_MIXIN(IntersectionObserverController);

 public:
  static IntersectionObserverController* Create(Document*);

  explicit IntersectionObserverController(Document*);
  ~IntersectionObserverController() override;

  void Unpause() override;

  void ScheduleIntersectionObserverForDelivery(IntersectionObserver&);
  void DeliverIntersectionObservations();
  void ComputeTrackedIntersectionObservations();
  void AddTrackedTarget(Element&);
  void RemoveTrackedTarget(Element&);

  void Trace(blink::Visitor*) override;
  const char* NameInHeapSnapshot() const override {
    return "IntersectionObserverController";
  }

 private:
  void PostTaskToDeliverObservations();

 private:
  // Elements in this document which are the target of an
  // IntersectionObservation.
  HeapHashSet<WeakMember<Element>> tracked_observation_targets_;
  // IntersectionObservers for which this is the execution context of the
  // callback.
  HeapHashSet<TraceWrapperMember<IntersectionObserver>>
      pending_intersection_observers_;
  // TODO(https://crbug.com/796145): Remove this hack once on-stack objects
  // get supported by either of wrapper-tracing or unified GC.
  HeapHashSet<TraceWrapperMember<IntersectionObserver>>
      intersection_observers_being_invoked_;

  bool callback_fired_while_suspended_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_INTERSECTION_OBSERVER_INTERSECTION_OBSERVER_CONTROLLER_H_
