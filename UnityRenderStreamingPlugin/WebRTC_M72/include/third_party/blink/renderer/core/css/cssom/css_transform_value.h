// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSSOM_CSS_TRANSFORM_VALUE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSSOM_CSS_TRANSFORM_VALUE_H_

#include "base/macros.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/css/cssom/css_style_value.h"
#include "third_party/blink/renderer/core/css/cssom/css_transform_component.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/heap_allocator.h"

namespace blink {

class DOMMatrix;

class CORE_EXPORT CSSTransformValue final : public CSSStyleValue {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static CSSTransformValue* Create(
      const HeapVector<Member<CSSTransformComponent>>& transform_components,
      ExceptionState&);

  // Blink-internal constructor
  static CSSTransformValue* Create(
      const HeapVector<Member<CSSTransformComponent>>& transform_components);

  static CSSTransformValue* FromCSSValue(const CSSValue&);

  CSSTransformValue(
      const HeapVector<Member<CSSTransformComponent>>& transform_components)
      : CSSStyleValue(), transform_components_(transform_components) {}

  bool is2D() const;

  DOMMatrix* toMatrix(ExceptionState&) const;

  const CSSValue* ToCSSValue() const override;

  StyleValueType GetType() const override { return kTransformType; }

  CSSTransformComponent* AnonymousIndexedGetter(wtf_size_t index) {
    return transform_components_.at(index);
  }
  bool AnonymousIndexedSetter(unsigned,
                              const Member<CSSTransformComponent>,
                              ExceptionState&);

  wtf_size_t length() const { return transform_components_.size(); }

  void Trace(blink::Visitor* visitor) override {
    visitor->Trace(transform_components_);
    CSSStyleValue::Trace(visitor);
  }

 private:
  HeapVector<Member<CSSTransformComponent>> transform_components_;
  DISALLOW_COPY_AND_ASSIGN(CSSTransformValue);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSSOM_CSS_TRANSFORM_VALUE_H_
