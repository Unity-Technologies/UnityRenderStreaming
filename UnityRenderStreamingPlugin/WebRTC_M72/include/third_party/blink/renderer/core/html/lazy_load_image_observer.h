// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_LAZY_LOAD_IMAGE_OBSERVER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_LAZY_LOAD_IMAGE_OBSERVER_H_

#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/heap/heap_allocator.h"
#include "third_party/blink/renderer/platform/heap/member.h"

namespace blink {

class Document;
class Element;
class HTMLImageElement;
class IntersectionObserver;
class IntersectionObserverEntry;
class Visitor;

class LazyLoadImageObserver final
    : public GarbageCollected<LazyLoadImageObserver> {
 public:
  struct VisibleLoadTimeMetrics {
    // Keeps track of whether the image was initially intersecting the viewport.
    bool is_initially_intersecting = false;
    bool has_initial_intersection_been_set = false;

    bool has_visibility_metrics_been_recorded = false;

    // Set when the image first becomes visible (i.e. appears in the viewport).
    TimeTicks time_when_first_visible;
  };

  LazyLoadImageObserver();

  static void StartMonitoring(Element*);
  static void StopMonitoring(Element*);

  static void StartTrackingVisibilityMetrics(HTMLImageElement*);
  static void RecordMetricsOnLoadFinished(HTMLImageElement*);

  void Trace(Visitor*);

 private:
  void StartMonitoringNearViewport(Document*, Element*);
  void LoadIfNearViewport(const HeapVector<Member<IntersectionObserverEntry>>&);

  void StartMonitoringVisibility(Document*, HTMLImageElement*);
  void OnLoadFinished(HTMLImageElement*);

  void OnVisibilityChanged(
      const HeapVector<Member<IntersectionObserverEntry>>&);

  // The intersection observer responsible for loading the image once it's near
  // the viewport.
  Member<IntersectionObserver> lazy_load_intersection_observer_;

  // The intersection observer used to track when the image becomes visible.
  Member<IntersectionObserver> visibility_metrics_observer_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_LAZY_LOAD_IMAGE_OBSERVER_H_
