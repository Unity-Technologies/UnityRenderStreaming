// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_PAINT_PROPERTY_TEST_HELPERS_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_PAINT_PROPERTY_TEST_HELPERS_H_

#include "third_party/blink/renderer/platform/graphics/paint/clip_paint_property_node.h"
#include "third_party/blink/renderer/platform/graphics/paint/effect_paint_property_node.h"
#include "third_party/blink/renderer/platform/graphics/paint/property_tree_state.h"
#include "third_party/blink/renderer/platform/graphics/paint/transform_paint_property_node.h"

namespace blink {

// Convenient shorthands.
inline const TransformPaintPropertyNode& t0() {
  return TransformPaintPropertyNode::Root();
}
inline const ClipPaintPropertyNode& c0() {
  return ClipPaintPropertyNode::Root();
}
inline const EffectPaintPropertyNode& e0() {
  return EffectPaintPropertyNode::Root();
}

inline scoped_refptr<EffectPaintPropertyNode> CreateOpacityEffect(
    const EffectPaintPropertyNode& parent,
    const TransformPaintPropertyNode* local_transform_space,
    const ClipPaintPropertyNode* output_clip,
    float opacity,
    CompositingReasons compositing_reasons = CompositingReason::kNone) {
  EffectPaintPropertyNode::State state;
  state.local_transform_space = local_transform_space;
  state.output_clip = output_clip;
  state.opacity = opacity;
  state.direct_compositing_reasons = compositing_reasons;
  return EffectPaintPropertyNode::Create(parent, std::move(state));
}

inline scoped_refptr<EffectPaintPropertyNode> CreateOpacityEffect(
    const EffectPaintPropertyNode& parent,
    float opacity,
    CompositingReasons compositing_reasons = CompositingReason::kNone) {
  return CreateOpacityEffect(parent, parent.Unalias()->LocalTransformSpace(),
                             parent.Unalias()->OutputClip(), opacity,
                             compositing_reasons);
}

inline scoped_refptr<EffectPaintPropertyNode> CreateFilterEffect(
    const EffectPaintPropertyNode& parent,
    const TransformPaintPropertyNode* local_transform_space,
    const ClipPaintPropertyNode* output_clip,
    CompositorFilterOperations filter,
    const FloatPoint& filters_origin = FloatPoint(),
    CompositingReasons compositing_reasons = CompositingReason::kNone) {
  EffectPaintPropertyNode::State state;
  state.local_transform_space = local_transform_space;
  state.output_clip = output_clip;
  state.filter = std::move(filter);
  state.filters_origin = filters_origin;
  state.direct_compositing_reasons = compositing_reasons;
  return EffectPaintPropertyNode::Create(parent, std::move(state));
}

inline scoped_refptr<EffectPaintPropertyNode> CreateFilterEffect(
    const EffectPaintPropertyNode& parent,
    CompositorFilterOperations filter,
    const FloatPoint& paint_offset = FloatPoint(),
    CompositingReasons compositing_reasons = CompositingReason::kNone) {
  return CreateFilterEffect(parent, parent.Unalias()->LocalTransformSpace(),
                            parent.Unalias()->OutputClip(), filter,
                            paint_offset, compositing_reasons);
}

inline scoped_refptr<ClipPaintPropertyNode> CreateClip(
    const ClipPaintPropertyNode& parent,
    const TransformPaintPropertyNode* local_transform_space,
    const FloatRoundedRect& clip_rect,
    CompositingReasons compositing_reasons = CompositingReason::kNone) {
  ClipPaintPropertyNode::State state;
  state.local_transform_space = local_transform_space;
  state.clip_rect = clip_rect;
  state.direct_compositing_reasons = compositing_reasons;
  return ClipPaintPropertyNode::Create(parent, std::move(state));
}

inline scoped_refptr<ClipPaintPropertyNode> CreateClipPathClip(
    const ClipPaintPropertyNode& parent,
    const TransformPaintPropertyNode* local_transform_space,
    const FloatRoundedRect& clip_rect) {
  ClipPaintPropertyNode::State state;
  state.local_transform_space = local_transform_space;
  state.clip_rect = clip_rect;
  state.clip_path = base::AdoptRef(new RefCountedPath);
  return ClipPaintPropertyNode::Create(parent, std::move(state));
}

inline scoped_refptr<TransformPaintPropertyNode> CreateTransform(
    const TransformPaintPropertyNode& parent,
    const TransformationMatrix& matrix,
    const FloatPoint3D& origin = FloatPoint3D(),
    CompositingReasons compositing_reasons = CompositingReason::kNone) {
  TransformPaintPropertyNode::State state;
  state.matrix = matrix;
  state.origin = origin;
  state.direct_compositing_reasons = compositing_reasons;
  return TransformPaintPropertyNode::Create(parent, std::move(state));
}

inline scoped_refptr<TransformPaintPropertyNode> CreateScrollTranslation(
    const TransformPaintPropertyNode& parent,
    float offset_x,
    float offset_y,
    const ScrollPaintPropertyNode& scroll,
    CompositingReasons compositing_reasons = CompositingReason::kNone) {
  TransformPaintPropertyNode::State state;
  state.matrix.Translate(offset_x, offset_y);
  state.direct_compositing_reasons = compositing_reasons;
  state.scroll = &scroll;
  return TransformPaintPropertyNode::Create(parent, std::move(state));
}

inline PropertyTreeState DefaultPaintChunkProperties() {
  return PropertyTreeState::Root();
}

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_PAINT_PROPERTY_TEST_HELPERS_H_
