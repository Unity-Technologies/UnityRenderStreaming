/*
 * Copyright (C) 2013 Adobe Systems Incorporated. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_SHAPES_RASTER_SHAPE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_SHAPES_RASTER_SHAPE_H_

#include <memory>
#include "base/macros.h"
#include "third_party/blink/renderer/core/layout/shapes/shape.h"
#include "third_party/blink/renderer/core/layout/shapes/shape_interval.h"
#include "third_party/blink/renderer/platform/geometry/float_rect.h"
#include "third_party/blink/renderer/platform/wtf/assertions.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class RasterShapeIntervals {
  USING_FAST_MALLOC(RasterShapeIntervals);

 public:
  RasterShapeIntervals(unsigned size, int offset = 0) : offset_(offset) {
    intervals_.resize(clampTo<int>(size));
  }

  void InitializeBounds();
  const IntRect& Bounds() const { return bounds_; }
  bool IsEmpty() const { return bounds_.IsEmpty(); }

  IntShapeInterval& IntervalAt(int y) {
    DCHECK_GE(y + offset_, 0);
    DCHECK_LT(static_cast<unsigned>(y + offset_), intervals_.size());
    return intervals_[y + offset_];
  }

  const IntShapeInterval& IntervalAt(int y) const {
    DCHECK_GE(y + offset_, 0);
    DCHECK_LT(static_cast<unsigned>(y + offset_), intervals_.size());
    return intervals_[y + offset_];
  }

  std::unique_ptr<RasterShapeIntervals> ComputeShapeMarginIntervals(
      int shape_margin) const;

  void BuildBoundsPath(Path&) const;

 private:
  int size() const { return intervals_.size(); }
  int Offset() const { return offset_; }
  int MinY() const { return -offset_; }
  int MaxY() const { return -offset_ + intervals_.size(); }

  IntRect bounds_;
  Vector<IntShapeInterval> intervals_;
  int offset_;
};

class RasterShape final : public Shape {
 public:
  RasterShape(std::unique_ptr<RasterShapeIntervals> intervals,
              const IntSize& margin_rect_size)
      : intervals_(std::move(intervals)), margin_rect_size_(margin_rect_size) {
    intervals_->InitializeBounds();
  }

  LayoutRect ShapeMarginLogicalBoundingBox() const override {
    return static_cast<LayoutRect>(MarginIntervals().Bounds());
  }
  bool IsEmpty() const override { return intervals_->IsEmpty(); }
  LineSegment GetExcludedInterval(LayoutUnit logical_top,
                                  LayoutUnit logical_height) const override;
  void BuildDisplayPaths(DisplayPaths& paths) const override {
    intervals_->BuildBoundsPath(paths.shape);
    if (ShapeMargin())
      MarginIntervals().BuildBoundsPath(paths.margin_shape);
  }

 private:
  const RasterShapeIntervals& MarginIntervals() const;

  std::unique_ptr<RasterShapeIntervals> intervals_;
  mutable std::unique_ptr<RasterShapeIntervals> margin_intervals_;
  IntSize margin_rect_size_;
  DISALLOW_COPY_AND_ASSIGN(RasterShape);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_SHAPES_RASTER_SHAPE_H_
