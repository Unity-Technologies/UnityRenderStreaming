// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_SIZE_INTERPOLATION_FUNCTIONS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_SIZE_INTERPOLATION_FUNCTIONS_H_

#include <memory>
#include "third_party/blink/renderer/core/animation/interpolation_value.h"
#include "third_party/blink/renderer/core/animation/pairwise_interpolation_value.h"
#include "third_party/blink/renderer/core/style/fill_layer.h"

namespace blink {

class CSSToLengthConversionData;
class CSSValue;

class SizeInterpolationFunctions {
  STATIC_ONLY(SizeInterpolationFunctions);

 public:
  static InterpolationValue ConvertFillSizeSide(const FillSize&,
                                                float zoom,
                                                bool convert_width);
  static InterpolationValue MaybeConvertCSSSizeSide(const CSSValue&,
                                                    bool convert_width);
  static PairwiseInterpolationValue MaybeMergeSingles(
      InterpolationValue&& start,
      InterpolationValue&& end);
  static InterpolationValue CreateNeutralValue(const NonInterpolableValue*);
  static bool NonInterpolableValuesAreCompatible(const NonInterpolableValue*,
                                                 const NonInterpolableValue*);
  static void Composite(std::unique_ptr<InterpolableValue>&,
                        scoped_refptr<NonInterpolableValue>&,
                        double underlying_fraction,
                        const InterpolableValue&,
                        const NonInterpolableValue*);
  static FillSize CreateFillSize(
      const InterpolableValue& interpolable_value_a,
      const NonInterpolableValue* non_interpolable_value_a,
      const InterpolableValue& interpolable_value_b,
      const NonInterpolableValue* non_interpolable_value_b,
      const CSSToLengthConversionData&);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_SIZE_INTERPOLATION_FUNCTIONS_H_
