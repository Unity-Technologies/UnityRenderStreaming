// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_ANIMATION_COMPOSITOR_SCROLL_OFFSET_ANIMATION_CURVE_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_ANIMATION_COMPOSITOR_SCROLL_OFFSET_ANIMATION_CURVE_H_

#include <memory>

#include "base/memory/ptr_util.h"
#include "third_party/blink/renderer/platform/animation/compositor_animation_curve.h"
#include "third_party/blink/renderer/platform/geometry/float_point.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/noncopyable.h"
#include "third_party/blink/renderer/platform/wtf/time.h"

namespace cc {
class ScrollOffsetAnimationCurve;
}

namespace blink {

class PLATFORM_EXPORT CompositorScrollOffsetAnimationCurve
    : public CompositorAnimationCurve {
  WTF_MAKE_NONCOPYABLE(CompositorScrollOffsetAnimationCurve);

 public:
  enum ScrollDurationBehavior {
    kScrollDurationDeltaBased = 0,
    kScrollDurationConstant,
    kScrollDurationInverseDelta
  };

  static std::unique_ptr<CompositorScrollOffsetAnimationCurve> Create(
      FloatPoint target_value,
      CompositorScrollOffsetAnimationCurve::ScrollDurationBehavior
          duration_behavior) {
    return base::WrapUnique(new CompositorScrollOffsetAnimationCurve(
        target_value, duration_behavior));
  }
  static std::unique_ptr<CompositorScrollOffsetAnimationCurve> Create(
      cc::ScrollOffsetAnimationCurve* curve) {
    return base::WrapUnique(new CompositorScrollOffsetAnimationCurve(curve));
  }

  ~CompositorScrollOffsetAnimationCurve() override;

  void SetInitialValue(FloatPoint);
  FloatPoint GetValue(double time) const;
  double Duration() const;
  FloatPoint TargetValue() const;
  void ApplyAdjustment(IntSize);
  void UpdateTarget(TimeDelta time, FloatPoint new_target);

  // CompositorAnimationCurve implementation.
  std::unique_ptr<cc::AnimationCurve> CloneToAnimationCurve() const override;

 private:
  CompositorScrollOffsetAnimationCurve(FloatPoint, ScrollDurationBehavior);
  CompositorScrollOffsetAnimationCurve(cc::ScrollOffsetAnimationCurve*);

  std::unique_ptr<cc::ScrollOffsetAnimationCurve> curve_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_ANIMATION_COMPOSITOR_SCROLL_OFFSET_ANIMATION_CURVE_H_
