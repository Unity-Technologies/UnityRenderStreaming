/*
 * Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_FE_COMPOSITE_ELEMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_FE_COMPOSITE_ELEMENT_H_

#include "third_party/blink/renderer/core/svg/svg_animated_enumeration.h"
#include "third_party/blink/renderer/core/svg/svg_animated_number.h"
#include "third_party/blink/renderer/core/svg/svg_filter_primitive_standard_attributes.h"
#include "third_party/blink/renderer/platform/graphics/filters/fe_composite.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

DECLARE_SVG_ENUM_MAP(CompositeOperationType);

class SVGFECompositeElement final
    : public SVGFilterPrimitiveStandardAttributes {
  DEFINE_WRAPPERTYPEINFO();

 public:
  DECLARE_NODE_FACTORY(SVGFECompositeElement);

  SVGAnimatedNumber* k1() { return k1_.Get(); }
  SVGAnimatedNumber* k2() { return k2_.Get(); }
  SVGAnimatedNumber* k3() { return k3_.Get(); }
  SVGAnimatedNumber* k4() { return k4_.Get(); }
  SVGAnimatedString* in1() { return in1_.Get(); }
  SVGAnimatedString* in2() { return in2_.Get(); }
  SVGAnimatedEnumeration<CompositeOperationType>* svgOperator() {
    return svg_operator_.Get();
  }

  void Trace(blink::Visitor*) override;

 private:
  explicit SVGFECompositeElement(Document&);

  bool SetFilterEffectAttribute(FilterEffect*, const QualifiedName&) override;
  void SvgAttributeChanged(const QualifiedName&) override;
  FilterEffect* Build(SVGFilterBuilder*, Filter*) override;

  Member<SVGAnimatedNumber> k1_;
  Member<SVGAnimatedNumber> k2_;
  Member<SVGAnimatedNumber> k3_;
  Member<SVGAnimatedNumber> k4_;
  Member<SVGAnimatedString> in1_;
  Member<SVGAnimatedString> in2_;
  Member<SVGAnimatedEnumeration<CompositeOperationType>> svg_operator_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_FE_COMPOSITE_ELEMENT_H_
