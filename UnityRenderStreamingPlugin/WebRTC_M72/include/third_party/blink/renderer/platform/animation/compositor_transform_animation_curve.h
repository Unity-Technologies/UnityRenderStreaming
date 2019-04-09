// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_ANIMATION_COMPOSITOR_TRANSFORM_ANIMATION_CURVE_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_ANIMATION_COMPOSITOR_TRANSFORM_ANIMATION_CURVE_H_

#include <memory>

#include "base/memory/ptr_util.h"
#include "third_party/blink/renderer/platform/animation/compositor_animation_curve.h"
#include "third_party/blink/renderer/platform/animation/compositor_transform_keyframe.h"
#include "third_party/blink/renderer/platform/animation/timing_function.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/noncopyable.h"

namespace cc {
class KeyframedTransformAnimationCurve;
}

namespace blink {
class CompositorTransformKeyframe;
}

namespace blink {

// A keyframed transform animation curve.
class PLATFORM_EXPORT CompositorTransformAnimationCurve
    : public CompositorAnimationCurve {
  WTF_MAKE_NONCOPYABLE(CompositorTransformAnimationCurve);

 public:
  static std::unique_ptr<CompositorTransformAnimationCurve> Create() {
    return base::WrapUnique(new CompositorTransformAnimationCurve());
  }

  ~CompositorTransformAnimationCurve() override;

  void AddKeyframe(const CompositorTransformKeyframe&);
  void SetTimingFunction(const TimingFunction&);
  void SetScaledDuration(double);

  // CompositorAnimationCurve implementation.
  std::unique_ptr<cc::AnimationCurve> CloneToAnimationCurve() const override;

 private:
  CompositorTransformAnimationCurve();

  std::unique_ptr<cc::KeyframedTransformAnimationCurve> curve_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_ANIMATION_COMPOSITOR_TRANSFORM_ANIMATION_CURVE_H_
