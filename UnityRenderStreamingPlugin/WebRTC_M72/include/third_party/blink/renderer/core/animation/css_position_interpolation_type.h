// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_CSS_POSITION_INTERPOLATION_TYPE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_CSS_POSITION_INTERPOLATION_TYPE_H_

#include "third_party/blink/renderer/core/animation/css_length_list_interpolation_type.h"

#include "third_party/blink/renderer/core/animation/css_position_axis_list_interpolation_type.h"
#include "third_party/blink/renderer/core/animation/list_interpolation_functions.h"
#include "third_party/blink/renderer/core/css/css_primitive_value.h"
#include "third_party/blink/renderer/core/css/css_value_pair.h"

namespace blink {

class CSSPositionInterpolationType : public CSSLengthListInterpolationType {
 public:
  CSSPositionInterpolationType(PropertyHandle property)
      : CSSLengthListInterpolationType(property) {}

 private:
  InterpolationValue MaybeConvertValue(const CSSValue& value,
                                       const StyleResolverState*,
                                       ConversionCheckers&) const final {
    if (!value.IsValuePair()) {
      return nullptr;
    }
    const CSSValuePair& pair = ToCSSValuePair(value);
    return ListInterpolationFunctions::CreateList(2, [&pair](size_t index) {
      return CSSPositionAxisListInterpolationType::ConvertPositionAxisCSSValue(
          index == 0 ? pair.First() : pair.Second());
    });
  }
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_CSS_POSITION_INTERPOLATION_TYPE_H_
