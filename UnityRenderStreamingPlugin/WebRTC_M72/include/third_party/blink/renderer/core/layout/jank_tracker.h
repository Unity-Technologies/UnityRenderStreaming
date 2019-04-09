// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_JANK_TRACKER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_JANK_TRACKER_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/layout/jank_region.h"
#include "third_party/blink/renderer/platform/geometry/region.h"
#include "third_party/blink/renderer/platform/timer.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace blink {

class LayoutObject;
class LayoutRect;
class LocalFrameView;
class PaintLayer;
class TracedValue;
class WebInputEvent;

// Tracks "jank" from layout objects changing their visual location between
// animation frames.
class CORE_EXPORT JankTracker {
  USING_FAST_MALLOC(JankTracker);

 public:
  JankTracker(LocalFrameView*);
  ~JankTracker() {}
  void NotifyObjectPrePaint(const LayoutObject& object,
                            const LayoutRect& old_visual_rect,
                            const PaintLayer& painting_layer);
  void NotifyCompositedLayerMoved(const PaintLayer&,
                                  FloatRect old_layer_rect,
                                  FloatRect new_layer_rect);
  void NotifyPrePaintFinished();
  void NotifyInput(const WebInputEvent&);
  bool IsActive();
  double Score() const { return score_; }
  float MaxDistance() const { return max_distance_; }
  void Dispose() { timer_.Stop(); }

 private:
  void AccumulateJank(const LayoutObject&,
                      const PaintLayer&,
                      FloatRect old_rect,
                      FloatRect new_rect);
  void TimerFired(TimerBase*) {}
  std::unique_ptr<TracedValue> PerFrameTraceData(
      double jank_fraction,
      double granularity_scale) const;

  // This owns us.
  UntracedMember<LocalFrameView> frame_view_;

  // The global jank score.
  double score_;

  // The per-frame jank region.
  Region region_;

  // Experimental jank region implementation using sweep-line algorithm.
  JankRegion region_experimental_;

  // Tracks the short period after an input event during which we ignore jank.
  TaskRunnerTimer<JankTracker> timer_;

  // The maximum distance any layout object has moved in any frame.
  float max_distance_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_JANK_TRACKER_H_
