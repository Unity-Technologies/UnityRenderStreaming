// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSSOM_CSS_KEYWORD_VALUE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSSOM_CSS_KEYWORD_VALUE_H_

#include "base/macros.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/css/cssom/css_style_value.h"
#include "third_party/blink/renderer/core/css_value_keywords.h"

namespace blink {

class ExceptionState;

class CORE_EXPORT CSSKeywordValue final : public CSSStyleValue {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static CSSKeywordValue* Create(const String& keyword);
  static CSSKeywordValue* Create(const String& keyword, ExceptionState&);
  static CSSKeywordValue* FromCSSValue(const CSSValue&);

  explicit CSSKeywordValue(const String& keyword) : keyword_value_(keyword) {}

  StyleValueType GetType() const override { return kKeywordType; }

  const String& value() const;
  void setValue(const String& keyword, ExceptionState&);
  CSSValueID KeywordValueID() const;

  const CSSValue* ToCSSValue() const override;

 private:
  String keyword_value_;
  DISALLOW_COPY_AND_ASSIGN(CSSKeywordValue);
};

DEFINE_TYPE_CASTS(CSSKeywordValue,
                  CSSStyleValue,
                  value,
                  value->GetType() ==
                      CSSStyleValue::StyleValueType::kKeywordType,
                  value.GetType() ==
                      CSSStyleValue::StyleValueType::kKeywordType);

}  // namespace blink

#endif
