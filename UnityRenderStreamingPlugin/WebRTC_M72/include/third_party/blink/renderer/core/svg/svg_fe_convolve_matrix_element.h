/*
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_FE_CONVOLVE_MATRIX_ELEMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_FE_CONVOLVE_MATRIX_ELEMENT_H_

#include "third_party/blink/renderer/core/svg/svg_animated_boolean.h"
#include "third_party/blink/renderer/core/svg/svg_animated_enumeration.h"
#include "third_party/blink/renderer/core/svg/svg_animated_integer.h"
#include "third_party/blink/renderer/core/svg/svg_animated_integer_optional_integer.h"
#include "third_party/blink/renderer/core/svg/svg_animated_number.h"
#include "third_party/blink/renderer/core/svg/svg_animated_number_list.h"
#include "third_party/blink/renderer/core/svg/svg_animated_number_optional_number.h"
#include "third_party/blink/renderer/core/svg/svg_filter_primitive_standard_attributes.h"
#include "third_party/blink/renderer/platform/graphics/filters/fe_convolve_matrix.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

DECLARE_SVG_ENUM_MAP(EdgeModeType);

class SVGFEConvolveMatrixElement final
    : public SVGFilterPrimitiveStandardAttributes {
  DEFINE_WRAPPERTYPEINFO();

 public:
  DECLARE_NODE_FACTORY(SVGFEConvolveMatrixElement);

  SVGAnimatedBoolean* preserveAlpha() { return preserve_alpha_.Get(); }
  SVGAnimatedNumber* divisor() { return divisor_.Get(); }
  SVGAnimatedNumber* bias() { return bias_.Get(); }
  SVGAnimatedNumber* kernelUnitLengthX() {
    return kernel_unit_length_->FirstNumber();
  }
  SVGAnimatedNumber* kernelUnitLengthY() {
    return kernel_unit_length_->SecondNumber();
  }
  SVGAnimatedNumberList* kernelMatrix() { return kernel_matrix_.Get(); }
  SVGAnimatedString* in1() { return in1_.Get(); }
  SVGAnimatedEnumeration<EdgeModeType>* edgeMode() { return edge_mode_.Get(); }
  SVGAnimatedInteger* orderX() const { return order_->FirstInteger(); }
  SVGAnimatedInteger* orderY() const { return order_->SecondInteger(); }
  SVGAnimatedInteger* targetX() { return target_x_.Get(); }
  SVGAnimatedInteger* targetY() { return target_y_.Get(); }

  void Trace(blink::Visitor*) override;

 private:
  explicit SVGFEConvolveMatrixElement(Document&);

  IntSize MatrixOrder() const;
  IntPoint TargetPoint() const;
  float ComputeDivisor() const;

  bool SetFilterEffectAttribute(FilterEffect*, const QualifiedName&) override;
  void SvgAttributeChanged(const QualifiedName&) override;
  FilterEffect* Build(SVGFilterBuilder*, Filter*) override;

  Member<SVGAnimatedNumber> bias_;
  Member<SVGAnimatedNumber> divisor_;
  Member<SVGAnimatedString> in1_;
  Member<SVGAnimatedEnumeration<EdgeModeType>> edge_mode_;
  Member<SVGAnimatedNumberList> kernel_matrix_;
  Member<SVGAnimatedNumberOptionalNumber> kernel_unit_length_;
  Member<SVGAnimatedIntegerOptionalInteger> order_;
  Member<SVGAnimatedBoolean> preserve_alpha_;
  Member<SVGAnimatedInteger> target_x_;
  Member<SVGAnimatedInteger> target_y_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_FE_CONVOLVE_MATRIX_ELEMENT_H_
