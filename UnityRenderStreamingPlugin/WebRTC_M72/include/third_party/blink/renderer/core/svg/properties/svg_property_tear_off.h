/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_PROPERTIES_SVG_PROPERTY_TEAR_OFF_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_PROPERTIES_SVG_PROPERTY_TEAR_OFF_H_

#include "third_party/blink/renderer/core/svg/properties/svg_property.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/bindings/trace_wrapper_member.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class ExceptionState;
class SVGAnimatedPropertyBase;
class SVGElement;

enum PropertyIsAnimValType { kPropertyIsNotAnimVal, kPropertyIsAnimVal };

class SVGPropertyTearOffBase : public ScriptWrappable {
 public:
  ~SVGPropertyTearOffBase() override = default;

  PropertyIsAnimValType PropertyIsAnimVal() const {
    return property_is_anim_val_;
  }

  bool IsAnimVal() const { return property_is_anim_val_ == kPropertyIsAnimVal; }
  bool IsImmutable() const { return IsAnimVal(); }

  virtual void CommitChange();

  SVGAnimatedPropertyBase* GetBinding() { return binding_; }
  SVGElement* ContextElement() const { return context_element_; }

  void Bind(SVGAnimatedPropertyBase* binding);

  void Trace(Visitor*) override;

  static void ThrowReadOnly(ExceptionState&);

 protected:
  SVGPropertyTearOffBase(SVGAnimatedPropertyBase* binding,
                         PropertyIsAnimValType property_is_anim_val);
  SVGPropertyTearOffBase(SVGElement* context_element);

  void EnsureAnimValUpdated();

 private:
  TraceWrapperMember<SVGElement> context_element_;
  Member<SVGAnimatedPropertyBase> binding_;
  PropertyIsAnimValType property_is_anim_val_;
};

template <typename Property>
class SVGPropertyTearOff : public SVGPropertyTearOffBase {
 public:
  Property* Target() {
    if (IsAnimVal())
      EnsureAnimValUpdated();

    return target_.Get();
  }

  void SetTarget(Property* target) { target_ = target; }

  void Trace(blink::Visitor* visitor) override {
    visitor->Trace(target_);
    SVGPropertyTearOffBase::Trace(visitor);
  }

 protected:
  SVGPropertyTearOff(Property* target,
                     SVGAnimatedPropertyBase* binding,
                     PropertyIsAnimValType property_is_anim_val)
      : SVGPropertyTearOffBase(binding, property_is_anim_val), target_(target) {
    DCHECK(target_);
  }
  SVGPropertyTearOff(Property* target, SVGElement* context_element)
      : SVGPropertyTearOffBase(context_element), target_(target) {
    DCHECK(target_);
  }

 private:
  Member<Property> target_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_PROPERTIES_SVG_PROPERTY_TEAR_OFF_H_
