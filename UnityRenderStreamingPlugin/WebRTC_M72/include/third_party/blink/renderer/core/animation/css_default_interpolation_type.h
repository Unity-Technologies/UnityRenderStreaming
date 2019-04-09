// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_CSS_DEFAULT_INTERPOLATION_TYPE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_CSS_DEFAULT_INTERPOLATION_TYPE_H_

#include "third_party/blink/renderer/core/animation/interpolation_type.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/heap/persistent.h"

namespace blink {

class CORE_EXPORT CSSDefaultNonInterpolableValue : public NonInterpolableValue {
 public:
  ~CSSDefaultNonInterpolableValue() final = default;

  static scoped_refptr<CSSDefaultNonInterpolableValue> Create(
      const CSSValue* css_value) {
    return base::AdoptRef(new CSSDefaultNonInterpolableValue(css_value));
  }

  const CSSValue* CssValue() const { return css_value_.Get(); }

  DECLARE_NON_INTERPOLABLE_VALUE_TYPE();

 private:
  CSSDefaultNonInterpolableValue(const CSSValue*);

  Persistent<const CSSValue> css_value_;
};

DEFINE_NON_INTERPOLABLE_VALUE_TYPE_CASTS(CSSDefaultNonInterpolableValue);

// Never supports pairwise conversion while always supporting single conversion.
// A catch all default for CSSValue interpolation.
class CSSDefaultInterpolationType : public InterpolationType {
 public:
  CSSDefaultInterpolationType(PropertyHandle property)
      : InterpolationType(property) {
    DCHECK(property.IsCSSProperty());
  }

  InterpolationValue MaybeConvertSingle(const PropertySpecificKeyframe&,
                                        const InterpolationEnvironment&,
                                        const InterpolationValue& underlying,
                                        ConversionCheckers&) const override;

  PairwiseInterpolationValue MaybeConvertPairwise(
      const PropertySpecificKeyframe& start_keyframe,
      const PropertySpecificKeyframe& end_keyframe,
      const InterpolationEnvironment&,
      const InterpolationValue& underlying,
      ConversionCheckers&) const final {
    return nullptr;
  }

  InterpolationValue MaybeConvertUnderlyingValue(
      const InterpolationEnvironment&) const final {
    return nullptr;
  }

  void Composite(UnderlyingValueOwner& underlying_value_owner,
                 double underlying_fraction,
                 const InterpolationValue& value,
                 double interpolation_fraction) const final {
    underlying_value_owner.Set(*this, value);
  }

  void Apply(const InterpolableValue&,
             const NonInterpolableValue*,
             InterpolationEnvironment&) const final;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_CSS_DEFAULT_INTERPOLATION_TYPE_H_
