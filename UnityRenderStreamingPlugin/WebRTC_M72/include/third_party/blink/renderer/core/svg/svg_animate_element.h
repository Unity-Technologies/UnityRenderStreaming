/*
 * Copyright (C) 2004, 2005 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2008 Apple Inc. All rights reserved.
 * Copyright (C) Research In Motion Limited 2011. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_ANIMATE_ELEMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_ANIMATE_ELEMENT_H_

#include <base/gtest_prod_util.h>
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/css/css_property_names.h"
#include "third_party/blink/renderer/core/svg/svg_animation_element.h"
#include "third_party/blink/renderer/core/svg_names.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

// If we have 'inherit' as animation value, we need to grab the value
// during the animation since the value can be animated itself.
enum AnimatedPropertyValueType { kRegularPropertyValue, kInheritValue };

class CORE_EXPORT SVGAnimateElement : public SVGAnimationElement {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static SVGAnimateElement* Create(Document&);

  SVGAnimateElement(const QualifiedName&, Document&);
  ~SVGAnimateElement() override;

  void Trace(blink::Visitor*) override;

  bool IsSVGAnimationAttributeSettingJavaScriptURL(
      const Attribute&) const override;

  AnimatedPropertyType GetAnimatedPropertyType();
  bool AnimatedPropertyTypeSupportsAddition();

 protected:
  bool HasValidTarget() override;

  void WillChangeAnimationTarget() final;
  void DidChangeAnimationTarget() final;

  void ResetAnimatedType() final;
  void ClearAnimatedType() final;

  bool CalculateToAtEndOfDurationValue(
      const String& to_at_end_of_duration_string) final;
  bool CalculateFromAndToValues(const String& from_string,
                                const String& to_string) final;
  bool CalculateFromAndByValues(const String& from_string,
                                const String& by_string) final;
  void CalculateAnimatedValue(float percentage,
                              unsigned repeat_count,
                              SVGSMILElement* result_element) final;
  void ApplyResultsToTarget() final;
  float CalculateDistance(const String& from_string,
                          const String& to_string) final;
  bool IsAdditive() final;

  void ParseAttribute(const AttributeModificationParams&) override;

  void SetAttributeName(const QualifiedName&);

  enum AttributeType {
    kAttributeTypeCSS,
    kAttributeTypeXML,
    kAttributeTypeAuto
  };
  AttributeType GetAttributeType() const { return attribute_type_; }

  FRIEND_TEST_ALL_PREFIXES(UnsafeSVGAttributeSanitizationTest,
                           stringsShouldNotSupportAddition);

 private:
  void ResetAnimatedPropertyType();

  bool ShouldApplyAnimation(const SVGElement& target_element,
                            const QualifiedName& attribute_name);

  void SetAttributeType(const AtomicString&);

  InsertionNotificationRequest InsertedInto(ContainerNode&) final;
  void RemovedFrom(ContainerNode&) final;

  virtual void ResolveTargetProperty();
  void ClearTargetProperty();

  virtual SVGPropertyBase* CreatePropertyForAnimation(const String&) const;
  SVGPropertyBase* CreatePropertyForAttributeAnimation(const String&) const;
  SVGPropertyBase* CreatePropertyForCSSAnimation(const String&) const;

  SVGPropertyBase* AdjustForInheritance(SVGPropertyBase*,
                                        AnimatedPropertyValueType) const;

  Member<SVGPropertyBase> from_property_;
  Member<SVGPropertyBase> to_property_;
  Member<SVGPropertyBase> to_at_end_of_duration_property_;
  Member<SVGPropertyBase> animated_value_;

 protected:
  Member<SVGAnimatedPropertyBase> target_property_;
  AnimatedPropertyType type_;
  CSSPropertyID css_property_id_;

  bool IsAnimatingSVGDom() const { return target_property_; }
  bool IsAnimatingCSSProperty() const {
    return css_property_id_ != CSSPropertyInvalid;
  }

 private:
  AnimatedPropertyValueType from_property_value_type_;
  AnimatedPropertyValueType to_property_value_type_;
  AttributeType attribute_type_;
};

inline bool IsSVGAnimateElement(const SVGElement& element) {
  return element.HasTagName(svg_names::kAnimateTag) ||
         element.HasTagName(svg_names::kAnimateTransformTag) ||
         element.HasTagName(svg_names::kSetTag);
}

DEFINE_SVGELEMENT_TYPE_CASTS_WITH_FUNCTION(SVGAnimateElement);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_ANIMATE_ELEMENT_H_
