/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Nikolas Zimmermann
 * <zimmermann@kde.org>
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_MARKER_ELEMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_MARKER_ELEMENT_H_

#include "third_party/blink/renderer/core/svg/svg_animated_angle.h"
#include "third_party/blink/renderer/core/svg/svg_animated_enumeration.h"
#include "third_party/blink/renderer/core/svg/svg_animated_length.h"
#include "third_party/blink/renderer/core/svg/svg_element.h"
#include "third_party/blink/renderer/core/svg/svg_fit_to_view_box.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

enum SVGMarkerUnitsType {
  kSVGMarkerUnitsUnknown = 0,
  kSVGMarkerUnitsUserSpaceOnUse,
  kSVGMarkerUnitsStrokeWidth
};
DECLARE_SVG_ENUM_MAP(SVGMarkerUnitsType);

class SVGMarkerElement final : public SVGElement, public SVGFitToViewBox {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(SVGMarkerElement);

 public:
  // Forward declare enumerations in the W3C naming scheme, for IDL generation.
  enum {
    kSvgMarkerunitsUnknown = kSVGMarkerUnitsUnknown,
    kSvgMarkerunitsUserspaceonuse = kSVGMarkerUnitsUserSpaceOnUse,
    kSvgMarkerunitsStrokewidth = kSVGMarkerUnitsStrokeWidth
  };

  enum {
    kSvgMarkerOrientUnknown = kSVGMarkerOrientUnknown,
    kSvgMarkerOrientAuto = kSVGMarkerOrientAuto,
    kSvgMarkerOrientAngle = kSVGMarkerOrientAngle
  };

  DECLARE_NODE_FACTORY(SVGMarkerElement);

  AffineTransform ViewBoxToViewTransform(float view_width,
                                         float view_height) const;

  void setOrientToAuto();
  void setOrientToAngle(SVGAngleTearOff*);

  SVGAnimatedLength* refX() const { return ref_x_.Get(); }
  SVGAnimatedLength* refY() const { return ref_y_.Get(); }
  SVGAnimatedLength* markerWidth() const { return marker_width_.Get(); }
  SVGAnimatedLength* markerHeight() const { return marker_height_.Get(); }
  SVGAnimatedEnumeration<SVGMarkerUnitsType>* markerUnits() {
    return marker_units_.Get();
  }
  SVGAnimatedAngle* orientAngle() { return orient_angle_.Get(); }
  SVGAnimatedEnumeration<SVGMarkerOrientType>* orientType() {
    return orient_angle_->OrientType();
  }

  void Trace(blink::Visitor*) override;

 private:
  explicit SVGMarkerElement(Document&);

  void SvgAttributeChanged(const QualifiedName&) override;
  void ChildrenChanged(const ChildrenChange&) override;

  LayoutObject* CreateLayoutObject(const ComputedStyle&) override;
  bool LayoutObjectIsNeeded(const ComputedStyle&) const override;

  bool SelfHasRelativeLengths() const override;

  Member<SVGAnimatedLength> ref_x_;
  Member<SVGAnimatedLength> ref_y_;
  Member<SVGAnimatedLength> marker_width_;
  Member<SVGAnimatedLength> marker_height_;
  Member<SVGAnimatedAngle> orient_angle_;
  Member<SVGAnimatedEnumeration<SVGMarkerUnitsType>> marker_units_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_MARKER_ELEMENT_H_
