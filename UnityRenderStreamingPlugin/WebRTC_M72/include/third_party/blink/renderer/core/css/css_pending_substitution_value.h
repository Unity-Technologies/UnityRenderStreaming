// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_PENDING_SUBSTITUTION_VALUE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_PENDING_SUBSTITUTION_VALUE_H_

#include "third_party/blink/renderer/core/css/css_property_names.h"
#include "third_party/blink/renderer/core/css/css_value.h"
#include "third_party/blink/renderer/core/css/css_variable_reference_value.h"

namespace blink {

class CSSPendingSubstitutionValue : public CSSValue {
 public:
  static CSSPendingSubstitutionValue* Create(
      CSSPropertyID shorthand_property_id,
      CSSVariableReferenceValue* shorthand_value) {
    return MakeGarbageCollected<CSSPendingSubstitutionValue>(
        shorthand_property_id, shorthand_value);
  }

  CSSPendingSubstitutionValue(CSSPropertyID shorthand_property_id,
                              CSSVariableReferenceValue* shorthand_value)
      : CSSValue(kPendingSubstitutionValueClass),
        shorthand_property_id_(shorthand_property_id),
        shorthand_value_(shorthand_value) {}

  CSSVariableReferenceValue* ShorthandValue() const {
    return shorthand_value_.Get();
  }

  CSSPropertyID ShorthandPropertyId() const { return shorthand_property_id_; }

  bool Equals(const CSSPendingSubstitutionValue& other) const {
    return shorthand_value_ == other.shorthand_value_;
  }
  String CustomCSSText() const;

  void TraceAfterDispatch(blink::Visitor*);

 private:
  CSSPropertyID shorthand_property_id_;
  Member<CSSVariableReferenceValue> shorthand_value_;
};

DEFINE_CSS_VALUE_TYPE_CASTS(CSSPendingSubstitutionValue,
                            IsPendingSubstitutionValue());

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_PENDING_SUBSTITUTION_VALUE_H_
