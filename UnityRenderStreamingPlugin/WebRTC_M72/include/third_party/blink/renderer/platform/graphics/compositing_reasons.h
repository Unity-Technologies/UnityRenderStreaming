// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_COMPOSITING_REASONS_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_COMPOSITING_REASONS_H_

#include <stdint.h>
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

using CompositingReasons = uint64_t;

#define FOR_EACH_COMPOSITING_REASON(V)                                        \
  /* Intrinsic reasons that can be known right away by the layer. */          \
  V(3DTransform)                                                              \
  V(Video)                                                                    \
  V(Canvas)                                                                   \
  V(Plugin)                                                                   \
  V(IFrame)                                                                   \
  V(BackfaceVisibilityHidden)                                                 \
  V(ActiveTransformAnimation)                                                 \
  V(ActiveOpacityAnimation)                                                   \
  V(ActiveFilterAnimation)                                                    \
  V(ActiveBackdropFilterAnimation)                                            \
  V(TransitionProperty)                                                       \
  V(ScrollDependentPosition)                                                  \
  V(OverflowScrollingTouch)                                                   \
  V(OverflowScrollingParent)                                                  \
  V(OutOfFlowClipping)                                                        \
  V(VideoOverlay)                                                             \
  V(WillChangeCompositingHint)                                                \
  V(BackdropFilter)                                                           \
  V(RootScroller)                                                             \
  V(ScrollTimelineTarget)                                                     \
                                                                              \
  /* Overlap reasons that require knowing what's behind you in paint-order    \
     before knowing the answer. */                                            \
  V(AssumedOverlap)                                                           \
  V(Overlap)                                                                  \
  V(NegativeZIndexChildren)                                                   \
  V(SquashingDisallowed)                                                      \
                                                                              \
  /* Subtree reasons that require knowing what the status of your subtree is  \
     before knowing the answer. */                                            \
  V(TransformWithCompositedDescendants)                                       \
  V(OpacityWithCompositedDescendants)                                         \
  V(MaskWithCompositedDescendants)                                            \
  V(ReflectionWithCompositedDescendants)                                      \
  V(FilterWithCompositedDescendants)                                          \
  V(BlendingWithCompositedDescendants)                                        \
  V(ClipsCompositingDescendants)                                              \
  V(PerspectiveWith3DDescendants)                                             \
  V(Preserve3DWith3DDescendants)                                              \
  V(ReflectionOfCompositedParent)                                             \
  V(IsolateCompositedDescendants)                                             \
  V(PositionFixedOrStickyWithCompositedDescendants)                           \
                                                                              \
  /* The root layer is a special case. It may be forced to be a layer, but it \
  also needs to be a layer if anything else in the subtree is composited. */  \
  V(Root)                                                                     \
                                                                              \
  /* CompositedLayerMapping internal hierarchy reasons. */                    \
  V(LayerForAncestorClip)                                                     \
  V(LayerForDescendantClip)                                                   \
  V(LayerForPerspective)                                                      \
  V(LayerForHorizontalScrollbar)                                              \
  V(LayerForVerticalScrollbar)                                                \
  V(LayerForOverflowControlsHost)                                             \
  V(LayerForScrollCorner)                                                     \
  V(LayerForScrollingContents)                                                \
  V(LayerForScrollingContainer)                                               \
  V(LayerForSquashingContents)                                                \
  V(LayerForSquashingContainer)                                               \
  V(LayerForForeground)                                                       \
  V(LayerForBackground)                                                       \
  V(LayerForMask)                                                             \
  V(LayerForClippingMask)                                                     \
  V(LayerForAncestorClippingMask)                                             \
  V(LayerForScrollingBlockSelection)                                          \
  /* Composited layer painted on top of all other layers as decoration. */    \
  V(LayerForDecoration)                                                       \
                                                                              \
  /* Composited elements with inline transforms trigger assumed overlap so    \
  that we can update their transforms quickly. */                             \
  V(InlineTransform)

class PLATFORM_EXPORT CompositingReason {
 private:
  // This contains ordinal values for compositing reasons and will be used to
  // generate the compositing reason bits.
  enum {
#define V(name) kE##name,
    FOR_EACH_COMPOSITING_REASON(V)
#undef V
  };

#define V(name) static_assert(kE##name < 64, "Should fit in 64 bits");
  FOR_EACH_COMPOSITING_REASON(V)
#undef V

 public:
  static Vector<const char*> ShortNames(CompositingReasons);
  static Vector<const char*> Descriptions(CompositingReasons);
  static String ToString(CompositingReasons);

  enum : CompositingReasons {
    kNone = 0,
    kAll = ~static_cast<CompositingReasons>(0),
#define V(name) k##name = UINT64_C(1) << kE##name,
    FOR_EACH_COMPOSITING_REASON(V)
#undef V

    // Various combinations of compositing reasons are defined here also, for
    // more intuitive and faster bitwise logic.
    kComboActiveAnimation =
        kActiveTransformAnimation | kActiveOpacityAnimation |
        kActiveFilterAnimation | kActiveBackdropFilterAnimation,

    kComboAllDirectStyleDeterminedReasons =
        k3DTransform | kBackfaceVisibilityHidden | kComboActiveAnimation |
        kTransitionProperty | kWillChangeCompositingHint | kBackdropFilter,

    kComboAllDirectNonStyleDeterminedReasons =
        kVideo | kCanvas | kPlugin | kIFrame | kOverflowScrollingParent |
        kOutOfFlowClipping | kVideoOverlay | kRoot | kRootScroller |
        kScrollDependentPosition | kScrollTimelineTarget,

    kComboAllDirectReasons = kComboAllDirectStyleDeterminedReasons |
                             kComboAllDirectNonStyleDeterminedReasons,

    kComboAllCompositedScrollingDeterminedReasons =
        kScrollDependentPosition | kOverflowScrollingTouch,

    kComboCompositedDescendants =
        kTransformWithCompositedDescendants | kIsolateCompositedDescendants |
        kOpacityWithCompositedDescendants | kMaskWithCompositedDescendants |
        kFilterWithCompositedDescendants | kBlendingWithCompositedDescendants |
        kReflectionWithCompositedDescendants | kClipsCompositingDescendants |
        kPositionFixedOrStickyWithCompositedDescendants,

    kCombo3DDescendants =
        kPreserve3DWith3DDescendants | kPerspectiveWith3DDescendants,

    kComboAllStyleDeterminedReasons = kComboAllDirectStyleDeterminedReasons |
                                      kComboCompositedDescendants |
                                      kCombo3DDescendants | kInlineTransform,

    kComboSquashableReasons =
        kOverlap | kAssumedOverlap | kOverflowScrollingParent,
  };
};

// Any reasons other than overlap or assumed overlap will require the layer to
// be separately compositing.
inline bool RequiresCompositing(CompositingReasons reasons) {
  return reasons & ~CompositingReason::kComboSquashableReasons;
}

// If the layer has overlap or assumed overlap, but no other reasons, then it
// should be squashed.
inline bool RequiresSquashing(CompositingReasons reasons) {
  return !RequiresCompositing(reasons) &&
         (reasons & CompositingReason::kComboSquashableReasons);
}

struct CompositingReasonsStats {
  size_t overlap_layers = 0;
  size_t active_animation_layers = 0;
  size_t assumed_overlap_layers = 0;
  size_t indirect_composited_layers = 0;
  size_t total_composited_layers = 0;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_COMPOSITING_REASONS_H_
