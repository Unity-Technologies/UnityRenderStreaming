/*
 * Copyright (C) 2004, 2005, 2006, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_GRADIENT_ELEMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_GRADIENT_ELEMENT_H_

#include "third_party/blink/renderer/core/inspector/inspector_trace_events.h"
#include "third_party/blink/renderer/core/svg/svg_animated_enumeration.h"
#include "third_party/blink/renderer/core/svg/svg_animated_transform_list.h"
#include "third_party/blink/renderer/core/svg/svg_element.h"
#include "third_party/blink/renderer/core/svg/svg_unit_types.h"
#include "third_party/blink/renderer/core/svg/svg_uri_reference.h"
#include "third_party/blink/renderer/core/svg_names.h"
#include "third_party/blink/renderer/platform/graphics/gradient.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

struct GradientAttributes;

enum SVGSpreadMethodType {
  kSVGSpreadMethodUnknown = 0,
  kSVGSpreadMethodPad,
  kSVGSpreadMethodReflect,
  kSVGSpreadMethodRepeat
};
DECLARE_SVG_ENUM_MAP(SVGSpreadMethodType);

class SVGGradientElement : public SVGElement, public SVGURIReference {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(SVGGradientElement);

 public:
  SVGAnimatedTransformList* gradientTransform() const {
    return gradient_transform_.Get();
  }
  SVGAnimatedEnumeration<SVGSpreadMethodType>* spreadMethod() const {
    return spread_method_.Get();
  }
  SVGAnimatedEnumeration<SVGUnitTypes::SVGUnitType>* gradientUnits() const {
    return gradient_units_.Get();
  }

  void InvalidateGradient(LayoutInvalidationReasonForTracing);
  void InvalidateDependentGradients();

  const SVGGradientElement* ReferencedElement() const;
  void CollectCommonAttributes(GradientAttributes&) const;

  void Trace(blink::Visitor*) override;

 protected:
  SVGGradientElement(const QualifiedName&, Document&);

  using VisitedSet = HeapHashSet<Member<const SVGGradientElement>>;

  void SvgAttributeChanged(const QualifiedName&) override;

 private:
  void CollectStyleForPresentationAttribute(
      const QualifiedName&,
      const AtomicString&,
      MutableCSSPropertyValueSet*) override;

  InsertionNotificationRequest InsertedInto(ContainerNode&) final;
  void RemovedFrom(ContainerNode&) final;
  void ChildrenChanged(const ChildrenChange&) final;

  void BuildPendingResource() override;
  void ClearResourceReferences();

  Vector<Gradient::ColorStop> BuildStops() const;

  Member<SVGAnimatedTransformList> gradient_transform_;
  Member<SVGAnimatedEnumeration<SVGSpreadMethodType>> spread_method_;
  Member<SVGAnimatedEnumeration<SVGUnitTypes::SVGUnitType>> gradient_units_;
  Member<IdTargetObserver> target_id_observer_;
};

inline bool IsSVGGradientElement(const SVGElement& element) {
  return element.HasTagName(svg_names::kRadialGradientTag) ||
         element.HasTagName(svg_names::kLinearGradientTag);
}

DEFINE_SVGELEMENT_TYPE_CASTS_WITH_FUNCTION(SVGGradientElement);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_GRADIENT_ELEMENT_H_
