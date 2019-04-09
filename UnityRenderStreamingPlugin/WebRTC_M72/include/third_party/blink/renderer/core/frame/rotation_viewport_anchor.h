// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_ROTATION_VIEWPORT_ANCHOR_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_ROTATION_VIEWPORT_ANCHOR_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/geometry/float_size.h"
#include "third_party/blink/renderer/platform/geometry/int_point.h"
#include "third_party/blink/renderer/platform/geometry/int_rect.h"
#include "third_party/blink/renderer/platform/geometry/layout_rect.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class LocalFrameView;
class Node;
class PageScaleConstraintsSet;
class ScrollableArea;
class VisualViewport;

// The rotation anchor provides a way to anchor a viewport origin to a DOM node.
// In particular, the user supplies an anchor point (in view coordinates, e.g.,
// (0, 0) == viewport origin, (0.5, 0) == viewport top center). The anchor point
// tracks the underlying DOM node; as the node moves or the view is resized, the
// viewport anchor maintains its orientation relative to the node, and the
// viewport origin maintains its orientation relative to the anchor. If there is
// no node or it is lost during the resize, we fall back to the resize anchor
// logic.
class CORE_EXPORT RotationViewportAnchor {
  STACK_ALLOCATED();

 public:
  RotationViewportAnchor(LocalFrameView& root_frame_view,
                         VisualViewport&,
                         const FloatSize& anchor_in_inner_view_coords,
                         PageScaleConstraintsSet&);
  ~RotationViewportAnchor();

 private:
  void SetAnchor();
  void RestoreToAnchor();

  FloatPoint GetInnerOrigin(const FloatSize& inner_size) const;

  void ComputeOrigins(const FloatSize& inner_size,
                      IntPoint& main_frame_offset,
                      FloatPoint& visual_viewport_offset) const;
  ScrollableArea& LayoutViewport() const;

  Member<LocalFrameView> root_frame_view_;
  Member<VisualViewport> visual_viewport_;

  float old_page_scale_factor_;
  float old_minimum_page_scale_factor_;

  // Inner viewport origin in the reference frame of the document in CSS pixels
  FloatPoint visual_viewport_in_document_;

  // Inner viewport origin in the reference frame of the outer viewport
  // normalized to the outer viewport size.
  FloatSize normalized_visual_viewport_offset_;

  Member<Node> anchor_node_;

  // In Document coordinates.
  LayoutRect anchor_node_bounds_;

  FloatSize anchor_in_inner_view_coords_;
  FloatSize anchor_in_node_coords_;

  Member<PageScaleConstraintsSet> page_scale_constraints_set_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_ROTATION_VIEWPORT_ANCHOR_H_
