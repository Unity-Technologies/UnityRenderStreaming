// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_NG_NG_INLINE_BOX_FRAGMENT_PAINTER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_NG_NG_INLINE_BOX_FRAGMENT_PAINTER_H_

#include "third_party/blink/renderer/core/layout/ng/geometry/ng_border_edges.h"
#include "third_party/blink/renderer/core/paint/inline_box_painter_base.h"
#include "third_party/blink/renderer/core/paint/ng/ng_box_fragment_painter.h"

#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/skia/include/core/SkBlendMode.h"

namespace blink {

class LayoutRect;
class NGPaintFragment;
struct PaintInfo;

// Painter for LayoutNG inline box fragments. Delegates to NGBoxFragmentPainter
// for all box painting logic that isn't specific to inline boxes.
class NGInlineBoxFragmentPainter : public InlineBoxPainterBase {
  STACK_ALLOCATED();

 public:
  NGInlineBoxFragmentPainter(const NGPaintFragment&);

  void Paint(const PaintInfo&, const LayoutPoint& paint_offset);
  void ComputeFragmentOffsetOnLine(TextDirection,
                                   LayoutUnit* offset_on_line,
                                   LayoutUnit* total_width) const;

 protected:
  LayoutRect PaintRectForImageStrip(const LayoutRect&,
                                    TextDirection direction) const override;

  BorderPaintingType GetBorderPaintType(
      const LayoutRect& adjusted_frame_rect,
      IntRect& adjusted_clip_rect,
      bool object_has_multiple_boxes) const override;
  void PaintNormalBoxShadow(const PaintInfo&,
                            const ComputedStyle&,
                            const LayoutRect& paint_rect) override;
  void PaintInsetBoxShadow(const PaintInfo&,
                           const ComputedStyle&,
                           const LayoutRect& paint_rect) override;

 private:
  void PaintBackgroundBorderShadow(const PaintInfo&,
                                   const LayoutPoint& paint_offset);

  const NGPaintFragment& inline_box_fragment_;
  NGBorderEdges border_edges_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_NG_NG_INLINE_BOX_FRAGMENT_PAINTER_H_
