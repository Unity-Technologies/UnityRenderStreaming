// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_JANK_REGION_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_JANK_REGION_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/geometry/int_rect.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

// Represents a per-frame jank region for JankTracker. Only used when the
// JankTrackingSweepLine feature is enabled.
//
// This class uses a sweep line algorithm to compute the area in O(n log n) time
// where n is the number of rects recorded by AddRect. For complex jank regions,
// this is more efficient than using blink::Region, which is worst-case O(n^2)
// from the repeated calls to Region::Unite.
//
// The high-level approach is described here:
// http://jeffe.cs.illinois.edu/open/klee.html
//
// The sweep line moves from left to right. (TODO: compare performance against a
// top-to-bottom sweep.)
//
// The sweep line's current intersection with the jank region ("active length")
// is tracked by a segment tree, similar to what is described at:
// https://en.wikipedia.org/wiki/Segment_tree
//
// There are some subtleties to the segment tree, which are described by the
// comments in the implementation.

class CORE_EXPORT JankRegion {
 public:
  void AddRect(const IntRect& rect) {
    if (!rect.IsEmpty())
      rects_.push_back(rect);
  }

  const Vector<IntRect>& GetRects() const { return rects_; }
  bool IsEmpty() const { return rects_.IsEmpty(); }
  void Reset() { rects_.clear(); }

  uint64_t Area() const;

 private:
  Vector<IntRect> rects_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_JANK_REGION_H_
