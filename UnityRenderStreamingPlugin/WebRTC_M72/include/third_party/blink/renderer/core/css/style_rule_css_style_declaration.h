/*
 * Copyright (C) 2012 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_STYLE_RULE_CSS_STYLE_DECLARATION_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_STYLE_RULE_CSS_STYLE_DECLARATION_H_

#include "third_party/blink/renderer/core/css/property_set_css_style_declaration.h"
#include "third_party/blink/renderer/platform/bindings/trace_wrapper_member.h"

namespace blink {

class CSSRule;
class MutableCSSPropertyValueSet;

class StyleRuleCSSStyleDeclaration : public PropertySetCSSStyleDeclaration {
 public:
  static StyleRuleCSSStyleDeclaration* Create(
      MutableCSSPropertyValueSet& property_set,
      CSSRule* parent_rule) {
    return MakeGarbageCollected<StyleRuleCSSStyleDeclaration>(property_set,
                                                              parent_rule);
  }

  StyleRuleCSSStyleDeclaration(MutableCSSPropertyValueSet&, CSSRule*);
  ~StyleRuleCSSStyleDeclaration() override;

  void Reattach(MutableCSSPropertyValueSet&);

  void Trace(blink::Visitor*) override;

 protected:
  CSSStyleSheet* ParentStyleSheet() const override;

  CSSRule* parentRule() const override { return parent_rule_; }

  void WillMutate() override;
  void DidMutate(MutationType) override;
  PropertyRegistry* GetPropertyRegistry() const final;

  TraceWrapperMember<CSSRule> parent_rule_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_STYLE_RULE_CSS_STYLE_DECLARATION_H_
