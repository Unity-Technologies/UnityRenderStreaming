// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_TRANSITION_INTERPOLATION_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_TRANSITION_INTERPOLATION_H_

#include "third_party/blink/renderer/core/animation/compositor_animations.h"
#include "third_party/blink/renderer/core/animation/interpolation.h"
#include "third_party/blink/renderer/core/animation/interpolation_type.h"
#include "third_party/blink/renderer/core/core_export.h"

namespace blink {

class StyleResolverState;
class InterpolationType;

// See the documentation of Interpolation for general information about this
// class hierarchy.
//
// The primary difference between TransitionInterpolation and other
// Interpolation subclasses is that it must store additional data required for
// retargeting transition effects that were sent to the compositor thread.
// Retargeting a transition involves interrupting an in-progress transition and
// creating a new transition from the current state to the new end state.
//
// The TransitionInterpolation subclass stores the start and end keyframes as
// InterpolationValue objects, with an InterpolationType object that applies to
// both InterpolationValues. It additionally stores AnimatableValue objects
// corresponding to start and end keyframes as communicated to the compositor
// thread. Together, this is equivalent to representing the start and end
// keyframes as TransitionPropertySpecificKeyframe objects with the added
// constraint that they share an InterpolationType.
// TODO(crbug.com/442163): Store information for communication with the
// compositor without using AnimatableValue objects.
//
// During the effect application phase of animation computation, the current
// value of the property is applied to the element by calling the Apply
// function.
class CORE_EXPORT TransitionInterpolation : public Interpolation {
 public:
  static TransitionInterpolation* Create(const PropertyHandle& property,
                                         const InterpolationType& type,
                                         InterpolationValue&& start,
                                         InterpolationValue&& end,
                                         AnimatableValue* compositor_start,
                                         AnimatableValue* compositor_end) {
    return new TransitionInterpolation(property, type, std::move(start),
                                       std::move(end), compositor_start,
                                       compositor_end);
  }

  void Apply(StyleResolverState&) const;

  bool IsTransitionInterpolation() const final { return true; }

  const PropertyHandle& GetProperty() const final { return property_; }

  std::unique_ptr<TypedInterpolationValue> GetInterpolatedValue() const;

  AnimatableValue* GetInterpolatedCompositorValue() const;

  void Interpolate(int iteration, double fraction) final;

  void Trace(Visitor* visitor) override {
    visitor->Trace(compositor_start_);
    visitor->Trace(compositor_end_);
    Interpolation::Trace(visitor);
  }

 protected:
  TransitionInterpolation(const PropertyHandle& property,
                          const InterpolationType& type,
                          InterpolationValue&& start,
                          InterpolationValue&& end,
                          AnimatableValue* compositor_start,
                          AnimatableValue* compositor_end)
      : property_(property),
        type_(type),
        start_(std::move(start)),
        end_(std::move(end)),
        merge_(type.MaybeMergeSingles(start_.Clone(), end_.Clone())),
        compositor_start_(compositor_start),
        compositor_end_(compositor_end) {
    // Incredibly speculative CHECKs, to try and get any insight on
    // crbug.com/826627. Somehow a crash is happening in this constructor, which
    // we believe is based on |start_| having no interpolable value. However a
    // CHECK added in TransitionKeyframe::SetValue isn't firing, so doing some
    // speculation here to try and broaden our understanding.
    // TODO(crbug.com/826627): Revert once bug is fixed.
    CHECK(start_);
    CHECK(merge_);
    cached_interpolable_value_ = merge_.start_interpolable_value->Clone();
    DCHECK_EQ(compositor_start_ && compositor_end_,
              property_.GetCSSProperty().IsCompositableProperty());
  }

 private:
  const InterpolableValue& CurrentInterpolableValue() const;
  NonInterpolableValue* CurrentNonInterpolableValue() const;

  const PropertyHandle property_;
  const InterpolationType& type_;
  const InterpolationValue start_;
  const InterpolationValue end_;
  const PairwiseInterpolationValue merge_;
  const Member<AnimatableValue> compositor_start_;
  const Member<AnimatableValue> compositor_end_;

  mutable double cached_fraction_ = 0;
  mutable int cached_iteration_ = 0;
  mutable std::unique_ptr<InterpolableValue> cached_interpolable_value_;
};

DEFINE_TYPE_CASTS(TransitionInterpolation,
                  Interpolation,
                  value,
                  value->IsTransitionInterpolation(),
                  value.IsTransitionInterpolation());

}  // namespace blink

#endif
