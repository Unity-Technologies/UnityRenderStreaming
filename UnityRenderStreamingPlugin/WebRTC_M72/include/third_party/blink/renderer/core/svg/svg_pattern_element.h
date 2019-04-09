/*
 * Copyright (C) 2004, 2005, 2006, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007 Rob Buis <buis@kde.org>
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_PATTERN_ELEMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_PATTERN_ELEMENT_H_

#include "third_party/blink/renderer/core/inspector/inspector_trace_events.h"
#include "third_party/blink/renderer/core/svg/svg_animated_enumeration.h"
#include "third_party/blink/renderer/core/svg/svg_animated_length.h"
#include "third_party/blink/renderer/core/svg/svg_animated_transform_list.h"
#include "third_party/blink/renderer/core/svg/svg_element.h"
#include "third_party/blink/renderer/core/svg/svg_fit_to_view_box.h"
#include "third_party/blink/renderer/core/svg/svg_tests.h"
#include "third_party/blink/renderer/core/svg/svg_unit_types.h"
#include "third_party/blink/renderer/core/svg/svg_uri_reference.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class SVGResource;
class PatternAttributes;

class SVGPatternElement final : public SVGElement,
                                public SVGURIReference,
                                public SVGTests,
                                public SVGFitToViewBox {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(SVGPatternElement);

 public:
  DECLARE_NODE_FACTORY(SVGPatternElement);

  void CollectPatternAttributes(PatternAttributes&) const;

  AffineTransform LocalCoordinateSpaceTransform(CTMScope) const override;

  SVGAnimatedLength* x() const { return x_.Get(); }
  SVGAnimatedLength* y() const { return y_.Get(); }
  SVGAnimatedLength* width() const { return width_.Get(); }
  SVGAnimatedLength* height() const { return height_.Get(); }
  SVGAnimatedTransformList* patternTransform() {
    return pattern_transform_.Get();
  }
  const SVGAnimatedTransformList* patternTransform() const {
    return pattern_transform_.Get();
  }
  SVGAnimatedEnumeration<SVGUnitTypes::SVGUnitType>* patternUnits() {
    return pattern_units_.Get();
  }
  SVGAnimatedEnumeration<SVGUnitTypes::SVGUnitType>* patternContentUnits() {
    return pattern_content_units_.Get();
  }
  const SVGAnimatedEnumeration<SVGUnitTypes::SVGUnitType>* patternUnits()
      const {
    return pattern_units_.Get();
  }
  const SVGAnimatedEnumeration<SVGUnitTypes::SVGUnitType>* patternContentUnits()
      const {
    return pattern_content_units_.Get();
  }

  void InvalidatePattern(LayoutInvalidationReasonForTracing);

  const SVGPatternElement* ReferencedElement() const;

  void Trace(blink::Visitor*) override;

 private:
  explicit SVGPatternElement(Document&);

  bool IsValid() const override { return SVGTests::IsValid(); }

  void CollectStyleForPresentationAttribute(
      const QualifiedName&,
      const AtomicString&,
      MutableCSSPropertyValueSet*) override;

  void SvgAttributeChanged(const QualifiedName&) override;
  InsertionNotificationRequest InsertedInto(ContainerNode&) final;
  void RemovedFrom(ContainerNode&) final;
  void ChildrenChanged(const ChildrenChange&) override;

  void BuildPendingResource() override;
  void ClearResourceReferences();

  LayoutObject* CreateLayoutObject(const ComputedStyle&) override;

  bool SelfHasRelativeLengths() const override;

  Member<SVGAnimatedLength> x_;
  Member<SVGAnimatedLength> y_;
  Member<SVGAnimatedLength> width_;
  Member<SVGAnimatedLength> height_;
  Member<SVGAnimatedTransformList> pattern_transform_;
  Member<SVGAnimatedEnumeration<SVGUnitTypes::SVGUnitType>> pattern_units_;
  Member<SVGAnimatedEnumeration<SVGUnitTypes::SVGUnitType>>
      pattern_content_units_;
  Member<SVGResource> resource_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_PATTERN_ELEMENT_H_
