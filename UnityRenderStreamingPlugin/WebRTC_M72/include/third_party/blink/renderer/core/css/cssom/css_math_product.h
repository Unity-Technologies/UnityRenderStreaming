// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSSOM_CSS_MATH_PRODUCT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSSOM_CSS_MATH_PRODUCT_H_

#include "base/macros.h"
#include "third_party/blink/renderer/core/css/cssom/css_math_variadic.h"

namespace blink {

// Represents the product of one or more CSSNumericValues.
// See CSSMathProduct.idl for more information about this class.
class CORE_EXPORT CSSMathProduct final : public CSSMathVariadic {
  DEFINE_WRAPPERTYPEINFO();

 public:
  // The constructor defined in the IDL.
  static CSSMathProduct* Create(const HeapVector<CSSNumberish>& args,
                                ExceptionState&);
  // Blink internal-constructor.
  static CSSMathProduct* Create(CSSNumericValueVector);

  CSSMathProduct(CSSNumericArray* values, const CSSNumericValueType& type)
      : CSSMathVariadic(values, type) {}

  String getOperator() const final { return "product"; }

  // From CSSStyleValue.
  StyleValueType GetType() const final { return CSSStyleValue::kProductType; }

  CSSCalcExpressionNode* ToCalcExpressionNode() const final;

 private:
  void BuildCSSText(Nested, ParenLess, StringBuilder&) const final;

  base::Optional<CSSNumericSumValue> SumValue() const final;
  DISALLOW_COPY_AND_ASSIGN(CSSMathProduct);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSSOM_CSS_MATH_PRODUCT_H_
