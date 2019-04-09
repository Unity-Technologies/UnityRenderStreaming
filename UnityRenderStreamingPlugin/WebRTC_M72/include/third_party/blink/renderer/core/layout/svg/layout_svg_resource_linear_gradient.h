/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_SVG_LAYOUT_SVG_RESOURCE_LINEAR_GRADIENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_SVG_LAYOUT_SVG_RESOURCE_LINEAR_GRADIENT_H_

#include "third_party/blink/renderer/core/layout/svg/layout_svg_resource_gradient.h"
#include "third_party/blink/renderer/core/svg/linear_gradient_attributes.h"

namespace blink {

class SVGLinearGradientElement;

class LayoutSVGResourceLinearGradient final : public LayoutSVGResourceGradient {
 public:
  explicit LayoutSVGResourceLinearGradient(SVGLinearGradientElement*);
  ~LayoutSVGResourceLinearGradient() override;

  const char* GetName() const override {
    return "LayoutSVGResourceLinearGradient";
  }

  static const LayoutSVGResourceType kResourceType =
      kLinearGradientResourceType;
  LayoutSVGResourceType ResourceType() const override { return kResourceType; }

  SVGUnitTypes::SVGUnitType GradientUnits() const override {
    return Attributes().GradientUnits();
  }
  AffineTransform CalculateGradientTransform() const override {
    return Attributes().GradientTransform();
  }
  bool CollectGradientAttributes() override;
  scoped_refptr<Gradient> BuildGradient() const override;

  FloatPoint StartPoint(const LinearGradientAttributes&) const;
  FloatPoint EndPoint(const LinearGradientAttributes&) const;

 private:
  Persistent<LinearGradientAttributesWrapper> attributes_wrapper_;

  LinearGradientAttributes& MutableAttributes() {
    return attributes_wrapper_->Attributes();
  }
  const LinearGradientAttributes& Attributes() const {
    return attributes_wrapper_->Attributes();
  }
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_SVG_LAYOUT_SVG_RESOURCE_LINEAR_GRADIENT_H_
