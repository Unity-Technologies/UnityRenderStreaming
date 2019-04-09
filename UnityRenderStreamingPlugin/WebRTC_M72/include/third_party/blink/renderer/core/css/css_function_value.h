// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_FUNCTION_VALUE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_FUNCTION_VALUE_H_

#include "third_party/blink/renderer/core/css/css_value_list.h"
#include "third_party/blink/renderer/core/css_value_keywords.h"

namespace blink {

class CSSFunctionValue : public CSSValueList {
 public:
  static CSSFunctionValue* Create(CSSValueID id) {
    return MakeGarbageCollected<CSSFunctionValue>(id);
  }

  CSSFunctionValue(CSSValueID id)
      : CSSValueList(kFunctionClass, kCommaSeparator), value_id_(id) {}

  String CustomCSSText() const;

  bool Equals(const CSSFunctionValue& other) const {
    return value_id_ == other.value_id_ && CSSValueList::Equals(other);
  }
  CSSValueID FunctionType() const { return value_id_; }

  void TraceAfterDispatch(blink::Visitor* visitor) {
    CSSValueList::TraceAfterDispatch(visitor);
  }

 private:
  const CSSValueID value_id_;
};

DEFINE_CSS_VALUE_TYPE_CASTS(CSSFunctionValue, IsFunctionValue());

}  // namespace blink

#endif
