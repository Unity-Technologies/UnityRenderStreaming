// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_PAINT_GEOMETRY_MAPPER_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_PAINT_GEOMETRY_MAPPER_H_

#include "base/optional.h"
#include "third_party/blink/renderer/platform/graphics/paint/float_clip_rect.h"
#include "third_party/blink/renderer/platform/graphics/paint/property_tree_state.h"
#include "third_party/blink/renderer/platform/scroll/scroll_types.h"
#include "third_party/blink/renderer/platform/wtf/hash_map.h"

namespace blink {

class TransformationMatrix;

// Clips can use FloatRect::Intersect or FloatRect::InclusiveIntersect.
enum InclusiveIntersectOrNot { kNonInclusiveIntersect, kInclusiveIntersect };

// GeometryMapper is a helper class for fast computations of transformed and
// visual rects in different PropertyTreeStates. The design document has a
// number of details on use cases, algorithmic definitions, and running times.
//
// NOTE: A GeometryMapper object is only valid for property trees that do not
// change. If any mutation occurs, a new GeometryMapper object must be allocated
// corresponding to the new state.
//
// ** WARNING** Callers to the methods below may not assume that any const
// references returned remain const across multiple calls into GeometryMapper.
// If needed, callers must store local copies of the return values.
//
// Design document: http://bit.ly/28P4FDA
class PLATFORM_EXPORT GeometryMapper {
  STATIC_ONLY(GeometryMapper);

 public:
  // Returns the matrix that is suitable to map geometries on the source plane
  // to some backing in the destination plane.
  // Formal definition:
  //   output = flatten(destination_to_screen)^-1 * flatten(source_to_screen)
  // There are some cases that flatten(destination_to_screen) being
  // singular yet we can still define a reasonable projection, for example:
  // 1. Both nodes inherited a common singular flat ancestor:
  // 2. Both nodes are co-planar to a common singular ancestor:
  // Not every cases outlined above are supported!
  // Read implementation comments for specific restrictions.
  static const TransformationMatrix& SourceToDestinationProjection(
      const TransformPaintPropertyNode* source,
      const TransformPaintPropertyNode* destination);

  // Same as SourceToDestinationProjection() except that it maps the rect
  // rather than returning the matrix.
  // |mapping_rect| is both input and output. Its type can be FloatRect,
  // LayoutRect or IntRect.
  template <typename Rect>
  ALWAYS_INLINE static void SourceToDestinationRect(
      const TransformPaintPropertyNode* source,
      const TransformPaintPropertyNode* destination,
      Rect& mapping_rect) {
    if (source == destination)
      return;

    // Fast-path optimization for mapping through just |source| when |source| is
    // a 2d translation.
    if (destination == source->Parent() &&
        source->IsIdentityOr2DTranslation()) {
      MoveRect(mapping_rect, source->Matrix().E(), source->Matrix().F());
      return;
    }

    // Fast-path optimization for mapping through just |destination| when
    // |destination| is a 2d translation.
    if (source == destination->Parent() &&
        destination->IsIdentityOr2DTranslation()) {
      MoveRect(mapping_rect, -destination->Matrix().E(),
               -destination->Matrix().F());
      return;
    }

    bool success = false;
    const TransformationMatrix& source_to_destination =
        SourceToDestinationProjectionInternal(source, destination, success);
    mapping_rect =
        success ? source_to_destination.MapRect(mapping_rect) : Rect();
  }

  // Returns the clip rect between |local_state| and |ancestor_state|. The clip
  // rect is the total clip rect that should be applied when painting contents
  // of |local_state| in |ancestor_state| space. Because this clip rect applies
  // on contents of |local_state|, it's not affected by any effect nodes between
  // |local_state| and |ancestor_state|.
  //
  // Note that the clip of |ancestor_state| is *not* applied.
  //
  // The output FloatClipRect may contain false positives for rounded-ness
  // if a rounded clip is clipped out, and overly conservative results
  // in the presences of transforms.

  static FloatClipRect LocalToAncestorClipRect(
      const PropertyTreeState& local_state,
      const PropertyTreeState& ancestor_state,
      OverlayScrollbarClipBehavior = kIgnorePlatformOverlayScrollbarSize);

  // Maps from a rect in |local_state| to its visual rect in |ancestor_state|.
  // If there is no effect node between |local_state| (included) and
  // |ancestor_state| (not included), the result is computed by multiplying the
  // rect by its combined transform between |local_state| and |ancestor_space|,
  // then flattening into 2D space, then intersecting by the clip for
  // |local_state|'s clips. If there are any pixel-moving effect nodes between
  // |local_state| and |ancestor_state|, for each segment of states separated
  // by the effect nodes, we'll execute the above process and map the result
  // rect with the effect.
  //
  // Note that the clip of |ancestor_state| is *not* applied.
  //
  // DCHECK fails if any of the paint property tree nodes in |local_state| are
  // not equal to or a descendant of that in |ancestor_state|.
  //
  // |mapping_rect| is both input and output.
  //
  // The output FloatClipRect may contain false positives for rounded-ness
  // if a rounded clip is clipped out, and overly conservative results
  // in the presences of transforms.
  //
  // Returns true if the mapped rect is non-empty. (Note: this has special
  // meaning in the presence of inclusive intersection.)
  //
  // Note: if inclusive intersection is specified, then the
  // GeometryMapperClipCache is bypassed (the GeometryMapperTRansformCache is
  // still used, however).
  //
  // If kInclusiveIntersect is set, clipping operations will
  // use FloatRect::InclusiveIntersect, and the return value of
  // InclusiveIntersect will be propagated to the return value of this method.
  // Otherwise, clipping operations will use LayoutRect::intersect, and the
  // return value will be true only if the clipped rect has non-zero area.
  // See the documentation for FloatRect::InclusiveIntersect for more
  // information.
  static bool LocalToAncestorVisualRect(
      const PropertyTreeState& local_state,
      const PropertyTreeState& ancestor_state,
      FloatClipRect& mapping_rect,
      OverlayScrollbarClipBehavior = kIgnorePlatformOverlayScrollbarSize,
      InclusiveIntersectOrNot = kNonInclusiveIntersect);

  // Returns true if |local_rect| is *not* clipped out by any clips
  // between |local_state| and |ancestor_state|. This includes not just
  // rectangular clips but rounded clips, and any clip paths stored on the
  // ClipPaintPropertyNodes. It does *not* include any "complex" clips (see
  // LayoutSVGResourceClipper::AsPath for the implementation of the heuristic
  // which differentiates "simple" from "complex".
  static bool PointVisibleInAncestorSpace(
      const PropertyTreeState& local_state,
      const PropertyTreeState& ancestor_state,
      const FloatPoint& local_point);

  static void ClearCache();

 private:
  // The internal methods do the same things as their public counterparts, but
  // take an extra |success| parameter which indicates if the function is
  // successful on return. See comments of the public functions for failure
  // conditions.

  static const TransformationMatrix& SourceToDestinationProjectionInternal(
      const TransformPaintPropertyNode* source,
      const TransformPaintPropertyNode* destination,
      bool& success);

  static FloatClipRect LocalToAncestorClipRectInternal(
      const ClipPaintPropertyNode* descendant,
      const ClipPaintPropertyNode* ancestor_clip,
      const TransformPaintPropertyNode* ancestor_transform,
      OverlayScrollbarClipBehavior,
      InclusiveIntersectOrNot,
      bool& success);

  // The return value has the same meaning as that for
  // LocalToAncestorVisualRect.
  static bool LocalToAncestorVisualRectInternal(
      const PropertyTreeState& local_state,
      const PropertyTreeState& ancestor_state,
      FloatClipRect& mapping_rect,
      OverlayScrollbarClipBehavior,
      InclusiveIntersectOrNot,
      bool& success);

  // The return value has the same meaning as that for
  // LocalToAncestorVisualRect.
  static bool SlowLocalToAncestorVisualRectWithEffects(
      const PropertyTreeState& local_state,
      const PropertyTreeState& ancestor_state,
      FloatClipRect& mapping_rect,
      OverlayScrollbarClipBehavior,
      InclusiveIntersectOrNot,
      bool& success);

  static void MoveRect(FloatRect& rect, double dx, double dy) {
    rect.Move(static_cast<float>(dx), static_cast<float>(dy));
  }

  static void MoveRect(LayoutRect& rect, double dx, double dy) {
    rect.Move(LayoutUnit(dx), LayoutUnit(dy));
  }

  static void MoveRect(IntRect& rect, double dx, double dy) {
    auto float_rect = FloatRect(rect);
    MoveRect(float_rect, dx, dy);
    rect = EnclosingIntRect(float_rect);
  }

  friend class GeometryMapperTest;
  friend class PaintLayerClipperTest;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_PAINT_GEOMETRY_MAPPER_H_
