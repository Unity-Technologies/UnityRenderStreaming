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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_PROPERTIES_SVG_ANIMATED_PROPERTY_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_PROPERTIES_SVG_ANIMATED_PROPERTY_H_

#include "base/macros.h"
#include "third_party/blink/renderer/core/css/css_property_names.h"
#include "third_party/blink/renderer/core/dom/qualified_name.h"
#include "third_party/blink/renderer/core/svg/properties/svg_property_info.h"
#include "third_party/blink/renderer/core/svg/properties/svg_property_tear_off.h"
#include "third_party/blink/renderer/core/svg/svg_parsing_error.h"
#include "third_party/blink/renderer/platform/bindings/trace_wrapper_member.h"
#include "third_party/blink/renderer/platform/heap/member.h"

namespace blink {

class ExceptionState;
class SVGElement;

class SVGAnimatedPropertyBase : public GarbageCollectedMixin {
 public:
  virtual ~SVGAnimatedPropertyBase();

  virtual SVGPropertyBase* CurrentValueBase() = 0;
  virtual const SVGPropertyBase& BaseValueBase() const = 0;
  virtual bool IsAnimating() const = 0;

  virtual SVGPropertyBase* CreateAnimatedValue() = 0;
  virtual void SetAnimatedValue(SVGPropertyBase*) = 0;
  virtual void AnimationEnded();

  virtual SVGParsingError AttributeChanged(const String&) = 0;
  virtual bool NeedsSynchronizeAttribute() const;
  virtual void SynchronizeAttribute();

  AnimatedPropertyType GetType() const {
    return static_cast<AnimatedPropertyType>(type_);
  }

  SVGElement* ContextElement() const { return context_element_; }

  const QualifiedName& AttributeName() const { return attribute_name_; }

  CSSPropertyID CssPropertyId() const {
    return static_cast<CSSPropertyID>(css_property_id_);
  }

  bool HasPresentationAttributeMapping() const {
    return CssPropertyId() != CSSPropertyInvalid;
  }

  bool IsSpecified() const;

  void Trace(Visitor*) override;

  void BaseValueChanged();
  void EnsureAnimValUpdated();

 protected:
  SVGAnimatedPropertyBase(AnimatedPropertyType,
                          SVGElement*,
                          const QualifiedName& attribute_name,
                          CSSPropertyID = CSSPropertyInvalid,
                          unsigned initial_value = 0);

  static constexpr int kInitialValueStorageBits = 3;
  unsigned InitialValueStorage() const { return initial_value_storage_; }

  void ClearBaseValueNeedsSynchronization() {
    base_value_needs_synchronization_ = false;
  }

 private:
  static_assert(kNumberOfAnimatedPropertyTypes <= (1u << 5),
                "enough bits for AnimatedPropertyType (type_)");
  static constexpr int kCssPropertyBits = 9;
  static_assert((1u << kCssPropertyBits) - 1 >= lastCSSProperty,
                "enough bits for CSS property ids");

  const unsigned type_ : 5;
  const unsigned css_property_id_ : kCssPropertyBits;
  const unsigned initial_value_storage_ : kInitialValueStorageBits;

  unsigned base_value_needs_synchronization_ : 1;
  TraceWrapperMember<SVGElement> context_element_;
  const QualifiedName& attribute_name_;
  DISALLOW_COPY_AND_ASSIGN(SVGAnimatedPropertyBase);
};

template <typename Property>
class SVGAnimatedPropertyCommon : public SVGAnimatedPropertyBase {
 public:
  Property* BaseValue() { return base_value_.Get(); }

  Property* CurrentValue() {
    return current_value_ ? current_value_.Get() : base_value_.Get();
  }

  const Property* CurrentValue() const {
    return const_cast<SVGAnimatedPropertyCommon*>(this)->CurrentValue();
  }

  SVGPropertyBase* CurrentValueBase() override { return CurrentValue(); }

  const SVGPropertyBase& BaseValueBase() const override { return *base_value_; }

  bool IsAnimating() const override { return current_value_; }

  SVGParsingError AttributeChanged(const String& value) override {
    static_assert(Property::kInitialValueBits <= kInitialValueStorageBits,
                  "enough bits for the initial value");

    ClearBaseValueNeedsSynchronization();
    const bool has_initial_value = Property::kInitialValueBits > 0;
    const bool is_attr_removal = value.IsNull();
    SVGParsingError parse_status = SVGParseStatus::kNoError;
    if (!has_initial_value || !is_attr_removal)
      parse_status = base_value_->SetValueAsString(value);
    if (has_initial_value &&
        (is_attr_removal || parse_status != SVGParseStatus::kNoError))
      base_value_->SetInitial(InitialValueStorage());
    return parse_status;
  }

  SVGPropertyBase* CreateAnimatedValue() override {
    return base_value_->Clone();
  }

  void SetAnimatedValue(SVGPropertyBase* value) override {
    DCHECK_EQ(value->GetType(), Property::ClassType());
    current_value_ = static_cast<Property*>(value);
  }

  void AnimationEnded() override {
    current_value_.Clear();

    SVGAnimatedPropertyBase::AnimationEnded();
  }

  void Trace(blink::Visitor* visitor) override {
    visitor->Trace(base_value_);
    visitor->Trace(current_value_);
    SVGAnimatedPropertyBase::Trace(visitor);
  }

 protected:
  SVGAnimatedPropertyCommon(SVGElement* context_element,
                            const QualifiedName& attribute_name,
                            Property* initial_value,
                            CSSPropertyID css_property_id = CSSPropertyInvalid,
                            unsigned initial_value_bits = 0)
      : SVGAnimatedPropertyBase(Property::ClassType(),
                                context_element,
                                attribute_name,
                                css_property_id,
                                initial_value_bits),
        base_value_(initial_value) {}

 private:
  Member<Property> base_value_;
  Member<Property> current_value_;
};

// Implementation of SVGAnimatedProperty which uses primitive types.
// This is for classes which return primitive type for its "animVal".
// Examples are SVGAnimatedBoolean, SVGAnimatedNumber, etc.
template <typename Property,
          typename TearOffType = typename Property::TearOffType,
          typename PrimitiveType = typename Property::PrimitiveType>
class SVGAnimatedProperty : public SVGAnimatedPropertyCommon<Property> {
 public:
  // SVGAnimated* DOM Spec implementations:

  // baseVal()/setBaseVal()/animVal() are only to be used from SVG DOM
  // implementation.  Use currentValue() from C++ code.
  PrimitiveType baseVal() { return this->BaseValue()->Value(); }

  void setBaseVal(PrimitiveType value, ExceptionState&) {
    this->BaseValue()->SetValue(value);
    this->BaseValueChanged();
  }

  PrimitiveType animVal() {
    this->EnsureAnimValUpdated();
    return this->CurrentValue()->Value();
  }

 protected:
  SVGAnimatedProperty(SVGElement* context_element,
                      const QualifiedName& attribute_name,
                      Property* initial_value,
                      CSSPropertyID css_property_id = CSSPropertyInvalid,
                      unsigned initial_value_bits = 0)
      : SVGAnimatedPropertyCommon<Property>(context_element,
                                            attribute_name,
                                            initial_value,
                                            css_property_id,
                                            initial_value_bits) {}
};

// Implementation of SVGAnimatedProperty which uses tear-off value types.
// These classes has "void" for its PrimitiveType.
// This is for classes which return special type for its "animVal".
// Examples are SVGAnimatedLength, SVGAnimatedRect, SVGAnimated*List, etc.
template <typename Property, typename TearOffType>
class SVGAnimatedProperty<Property, TearOffType, void>
    : public SVGAnimatedPropertyCommon<Property> {
 public:
  static SVGAnimatedProperty<Property>* Create(
      SVGElement* context_element,
      const QualifiedName& attribute_name,
      Property* initial_value,
      CSSPropertyID css_property_id = CSSPropertyInvalid) {
    return new SVGAnimatedProperty<Property>(context_element, attribute_name,
                                             initial_value, css_property_id);
  }

  void SetAnimatedValue(SVGPropertyBase* value) override {
    SVGAnimatedPropertyCommon<Property>::SetAnimatedValue(value);
    UpdateAnimValTearOffIfNeeded();
  }

  void AnimationEnded() override {
    SVGAnimatedPropertyCommon<Property>::AnimationEnded();
    UpdateAnimValTearOffIfNeeded();
  }

  // SVGAnimated* DOM Spec implementations:

  // baseVal()/animVal() are only to be used from SVG DOM implementation.
  // Use currentValue() from C++ code.
  virtual TearOffType* baseVal() {
    if (!base_val_tear_off_) {
      base_val_tear_off_ =
          TearOffType::Create(this->BaseValue(), this, kPropertyIsNotAnimVal);
    }
    return base_val_tear_off_;
  }

  TearOffType* animVal() {
    if (!anim_val_tear_off_) {
      anim_val_tear_off_ =
          TearOffType::Create(this->CurrentValue(), this, kPropertyIsAnimVal);
    }
    return anim_val_tear_off_;
  }

  void Trace(blink::Visitor* visitor) override {
    visitor->Trace(base_val_tear_off_);
    visitor->Trace(anim_val_tear_off_);
    SVGAnimatedPropertyCommon<Property>::Trace(visitor);
  }

 protected:
  SVGAnimatedProperty(SVGElement* context_element,
                      const QualifiedName& attribute_name,
                      Property* initial_value,
                      CSSPropertyID css_property_id = CSSPropertyInvalid,
                      unsigned initial_value_bits = 0)
      : SVGAnimatedPropertyCommon<Property>(context_element,
                                            attribute_name,
                                            initial_value,
                                            css_property_id,
                                            initial_value_bits) {}

 private:
  void UpdateAnimValTearOffIfNeeded() {
    if (anim_val_tear_off_)
      anim_val_tear_off_->SetTarget(this->CurrentValue());
  }

  // When still (not animated):
  //     Both m_animValTearOff and m_baseValTearOff target m_baseValue.
  // When animated:
  //     m_animValTearOff targets m_currentValue.
  //     m_baseValTearOff targets m_baseValue.
  Member<TearOffType> base_val_tear_off_;
  Member<TearOffType> anim_val_tear_off_;
};

// Implementation of SVGAnimatedProperty which doesn't use tear-off value types.
// This class has "void" for its TearOffType.
// Currently only used for SVGAnimatedPath.
template <typename Property>
class SVGAnimatedProperty<Property, void, void>
    : public SVGAnimatedPropertyCommon<Property> {
 public:
  static SVGAnimatedProperty<Property>* Create(
      SVGElement* context_element,
      const QualifiedName& attribute_name,
      Property* initial_value,
      CSSPropertyID css_property_id = CSSPropertyInvalid) {
    return new SVGAnimatedProperty<Property>(context_element, attribute_name,
                                             initial_value, css_property_id);
  }

 protected:
  SVGAnimatedProperty(SVGElement* context_element,
                      const QualifiedName& attribute_name,
                      Property* initial_value,
                      CSSPropertyID css_property_id = CSSPropertyInvalid)
      : SVGAnimatedPropertyCommon<Property>(context_element,
                                            attribute_name,
                                            initial_value,
                                            css_property_id) {}
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_PROPERTIES_SVG_ANIMATED_PROPERTY_H_
