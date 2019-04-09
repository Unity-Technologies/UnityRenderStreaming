// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_TYPED_INTERPOLATION_VALUE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_TYPED_INTERPOLATION_VALUE_H_

#include <memory>
#include <utility>

#include "base/memory/ptr_util.h"
#include "third_party/blink/renderer/core/animation/interpolation_value.h"

namespace blink {

class InterpolationType;

// Represents an interpolated value between an adjacent pair of
// PropertySpecificKeyframes.
class TypedInterpolationValue {
 public:
  static std::unique_ptr<TypedInterpolationValue> Create(
      const InterpolationType& type,
      std::unique_ptr<InterpolableValue> interpolable_value,
      scoped_refptr<NonInterpolableValue> non_interpolable_value = nullptr) {
    return base::WrapUnique(
        new TypedInterpolationValue(type, std::move(interpolable_value),
                                    std::move(non_interpolable_value)));
  }

  std::unique_ptr<TypedInterpolationValue> Clone() const {
    InterpolationValue copy = value_.Clone();
    return Create(type_, std::move(copy.interpolable_value),
                  std::move(copy.non_interpolable_value));
  }

  const InterpolationType& GetType() const { return type_; }
  const InterpolableValue& GetInterpolableValue() const {
    return *value_.interpolable_value;
  }
  const NonInterpolableValue* GetNonInterpolableValue() const {
    return value_.non_interpolable_value.get();
  }
  const InterpolationValue& Value() const { return value_; }

  InterpolationValue& MutableValue() { return value_; }

 private:
  TypedInterpolationValue(
      const InterpolationType& type,
      std::unique_ptr<InterpolableValue> interpolable_value,
      scoped_refptr<NonInterpolableValue> non_interpolable_value)
      : type_(type),
        value_(std::move(interpolable_value),
               std::move(non_interpolable_value)) {
    DCHECK(value_.interpolable_value);
  }

  const InterpolationType& type_;
  InterpolationValue value_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_TYPED_INTERPOLATION_VALUE_H_
