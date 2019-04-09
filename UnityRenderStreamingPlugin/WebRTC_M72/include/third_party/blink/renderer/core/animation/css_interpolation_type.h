// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_CSS_INTERPOLATION_TYPE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_CSS_INTERPOLATION_TYPE_H_

#include "third_party/blink/renderer/core/animation/css_interpolation_environment.h"
#include "third_party/blink/renderer/core/animation/interpolation_type.h"
#include "third_party/blink/renderer/core/core_export.h"

namespace blink {

class CSSCustomPropertyDeclaration;
class CSSVariableResolver;
class ComputedStyle;
class PropertyRegistration;
class StyleResolverState;

class CORE_EXPORT CSSInterpolationType : public InterpolationType {
 public:
  class CSSConversionChecker : public ConversionChecker {
   public:
    bool IsValid(const InterpolationEnvironment& environment,
                 const InterpolationValue& underlying) const final {
      return IsValid(ToCSSInterpolationEnvironment(environment).GetState(),
                     underlying);
    }

   protected:
    virtual bool IsValid(const StyleResolverState&,
                         const InterpolationValue& underlying) const = 0;
  };

  virtual InterpolationValue MaybeConvertNeutral(
      const InterpolationValue& underlying,
      ConversionCheckers&) const = 0;
  virtual InterpolationValue MaybeConvertInitial(const StyleResolverState&,
                                                 ConversionCheckers&) const = 0;
  virtual InterpolationValue MaybeConvertInherit(const StyleResolverState&,
                                                 ConversionCheckers&) const = 0;
  virtual InterpolationValue MaybeConvertValue(const CSSValue&,
                                               const StyleResolverState*,
                                               ConversionCheckers&) const = 0;
  virtual const CSSValue* CreateCSSValue(const InterpolableValue&,
                                         const NonInterpolableValue*,
                                         const StyleResolverState&) const {
    // TODO(alancutter): Implement this for all subclasses and make this an
    // abstract declaration so the return type can be changed to
    // const CSSValue&.
    NOTREACHED();
    return nullptr;
  }

 protected:
  CSSInterpolationType(PropertyHandle, const PropertyRegistration* = nullptr);

  const CSSProperty& CssProperty() const {
    return GetProperty().GetCSSProperty();
  }

  InterpolationValue MaybeConvertSingle(const PropertySpecificKeyframe&,
                                        const InterpolationEnvironment&,
                                        const InterpolationValue& underlying,
                                        ConversionCheckers&) const final;

  virtual void AdditiveKeyframeHook(InterpolationValue&) const {}

  InterpolationValue MaybeConvertUnderlyingValue(
      const InterpolationEnvironment&) const final;
  virtual InterpolationValue MaybeConvertStandardPropertyUnderlyingValue(
      const ComputedStyle&) const = 0;

  void Apply(const InterpolableValue&,
             const NonInterpolableValue*,
             InterpolationEnvironment&) const final;
  virtual void ApplyStandardPropertyValue(const InterpolableValue&,
                                          const NonInterpolableValue*,
                                          StyleResolverState&) const = 0;

 private:
  InterpolationValue MaybeConvertSingleInternal(
      const PropertySpecificKeyframe&,
      const InterpolationEnvironment&,
      const InterpolationValue& underlying,
      ConversionCheckers&) const;

  InterpolationValue MaybeConvertCustomPropertyDeclaration(
      const CSSCustomPropertyDeclaration&,
      const StyleResolverState&,
      CSSVariableResolver&,
      ConversionCheckers&) const;

  const PropertyRegistration& Registration() const {
    DCHECK(GetProperty().IsCSSCustomProperty());
    return *registration_;
  }

  void ApplyCustomPropertyValue(const InterpolableValue&,
                                const NonInterpolableValue*,
                                StyleResolverState&) const;

  WeakPersistent<const PropertyRegistration> registration_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_CSS_INTERPOLATION_TYPE_H_
