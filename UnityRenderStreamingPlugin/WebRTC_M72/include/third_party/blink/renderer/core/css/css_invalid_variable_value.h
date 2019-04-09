// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_INVALID_VARIABLE_VALUE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_INVALID_VARIABLE_VALUE_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/css/css_value.h"

namespace blink {

class CORE_EXPORT CSSInvalidVariableValue : public CSSValue {
 public:
  static CSSInvalidVariableValue* Create();

  String CustomCSSText() const;

  bool Equals(const CSSInvalidVariableValue&) const { return true; }

  void TraceAfterDispatch(blink::Visitor* visitor) {
    CSSValue::TraceAfterDispatch(visitor);
  }

 private:
  friend class CSSValuePool;

  CSSInvalidVariableValue() : CSSValue(kInvalidVariableValueClass) {}
};

DEFINE_CSS_VALUE_TYPE_CASTS(CSSInvalidVariableValue, IsInvalidVariableValue());

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_INVALID_VARIABLE_VALUE_H_
