// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_LIST_INTERPOLATION_FUNCTIONS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_LIST_INTERPOLATION_FUNCTIONS_H_

#include <memory>
#include "third_party/blink/renderer/core/animation/interpolation_value.h"
#include "third_party/blink/renderer/core/animation/pairwise_interpolation_value.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class UnderlyingValueOwner;
class InterpolationType;

class CORE_EXPORT ListInterpolationFunctions {
 public:
  template <typename CreateItemCallback>
  static InterpolationValue CreateList(wtf_size_t length, CreateItemCallback);
  static InterpolationValue CreateEmptyList() {
    return InterpolationValue(InterpolableList::Create(0));
  }

  enum class LengthMatchingStrategy {
    kEqual,
    kLowestCommonMultiple,
    kPadToLargest
  };

  using MergeSingleItemConversionsCallback =
      base::RepeatingCallback<PairwiseInterpolationValue(InterpolationValue&&,
                                                         InterpolationValue&&)>;

  static PairwiseInterpolationValue MaybeMergeSingles(
      InterpolationValue&& start,
      InterpolationValue&& end,
      LengthMatchingStrategy,
      MergeSingleItemConversionsCallback);

  using EqualNonInterpolableValuesCallback =
      bool (*)(const NonInterpolableValue*, const NonInterpolableValue*);
  static bool EqualValues(const InterpolationValue&,
                          const InterpolationValue&,
                          EqualNonInterpolableValuesCallback);

  using NonInterpolableValuesAreCompatibleCallback =
      base::RepeatingCallback<bool(const NonInterpolableValue*,
                                   const NonInterpolableValue*)>;
  using CompositeItemCallback =
      base::RepeatingCallback<void(std::unique_ptr<InterpolableValue>&,
                                   scoped_refptr<NonInterpolableValue>&,
                                   double underlying_fraction,
                                   const InterpolableValue&,
                                   const NonInterpolableValue*)>;
  static void Composite(UnderlyingValueOwner&,
                        double underlying_fraction,
                        const InterpolationType&,
                        const InterpolationValue&,
                        LengthMatchingStrategy,
                        NonInterpolableValuesAreCompatibleCallback,
                        CompositeItemCallback);
};

class CORE_EXPORT NonInterpolableList : public NonInterpolableValue {
 public:
  ~NonInterpolableList() final = default;

  static scoped_refptr<NonInterpolableList> Create() {
    return base::AdoptRef(new NonInterpolableList());
  }
  static scoped_refptr<NonInterpolableList> Create(
      Vector<scoped_refptr<NonInterpolableValue>>&& list) {
    return base::AdoptRef(new NonInterpolableList(std::move(list)));
  }

  wtf_size_t length() const { return list_.size(); }
  const NonInterpolableValue* Get(wtf_size_t index) const {
    return list_[index].get();
  }
  NonInterpolableValue* Get(wtf_size_t index) { return list_[index].get(); }
  scoped_refptr<NonInterpolableValue>& GetMutable(wtf_size_t index) {
    return list_[index];
  }

  DECLARE_NON_INTERPOLABLE_VALUE_TYPE();

 private:
  NonInterpolableList() = default;
  NonInterpolableList(Vector<scoped_refptr<NonInterpolableValue>>&& list)
      : list_(list) {}

  Vector<scoped_refptr<NonInterpolableValue>> list_;
};

DEFINE_NON_INTERPOLABLE_VALUE_TYPE_CASTS(NonInterpolableList);

template <typename CreateItemCallback>
InterpolationValue ListInterpolationFunctions::CreateList(
    wtf_size_t length,
    CreateItemCallback create_item) {
  if (length == 0)
    return CreateEmptyList();
  std::unique_ptr<InterpolableList> interpolable_list =
      InterpolableList::Create(length);
  Vector<scoped_refptr<NonInterpolableValue>> non_interpolable_values(length);
  for (wtf_size_t i = 0; i < length; i++) {
    InterpolationValue item = create_item(i);
    if (!item)
      return nullptr;
    interpolable_list->Set(i, std::move(item.interpolable_value));
    non_interpolable_values[i] = std::move(item.non_interpolable_value);
  }
  return InterpolationValue(
      std::move(interpolable_list),
      NonInterpolableList::Create(std::move(non_interpolable_values)));
}

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_LIST_INTERPOLATION_FUNCTIONS_H_
