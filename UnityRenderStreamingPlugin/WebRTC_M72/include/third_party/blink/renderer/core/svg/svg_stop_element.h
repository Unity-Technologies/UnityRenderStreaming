/*
 * Copyright (C) 2004, 2005, 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_STOP_ELEMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_STOP_ELEMENT_H_

#include "third_party/blink/renderer/core/svg/svg_animated_number.h"
#include "third_party/blink/renderer/core/svg/svg_element.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class Color;

class SVGStopElement final : public SVGElement {
  DEFINE_WRAPPERTYPEINFO();

 public:
  DECLARE_NODE_FACTORY(SVGStopElement);

  Color StopColorIncludingOpacity() const;

  SVGAnimatedNumber* offset() const { return offset_.Get(); }

  void Trace(blink::Visitor*) override;

 protected:
  void DidRecalcStyle(StyleRecalcChange) override;

 private:
  explicit SVGStopElement(Document&);

  void SvgAttributeChanged(const QualifiedName&) override;

  // Stop elements don't have associated layout objects
  // (they use NonLayoutObjectComputedStyle instead).
  bool LayoutObjectIsNeeded(const ComputedStyle&) const override {
    return false;
  }

  Member<SVGAnimatedNumber> offset_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_STOP_ELEMENT_H_
