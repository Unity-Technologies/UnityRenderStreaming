/*
 * Copyright (C) 2004, 2005 Nikolas Zimmermann <zimmermann@kde.org>
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_FE_MERGE_NODE_ELEMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_FE_MERGE_NODE_ELEMENT_H_

#include "third_party/blink/renderer/core/svg/svg_animated_string.h"
#include "third_party/blink/renderer/core/svg/svg_element.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class SVGFEMergeNodeElement final : public SVGElement {
  DEFINE_WRAPPERTYPEINFO();

 public:
  DECLARE_NODE_FACTORY(SVGFEMergeNodeElement);
  SVGAnimatedString* in1() { return in1_.Get(); }

  void Trace(blink::Visitor*) override;

 private:
  explicit SVGFEMergeNodeElement(Document&);

  void SvgAttributeChanged(const QualifiedName&) override;

  bool LayoutObjectIsNeeded(const ComputedStyle&) const override {
    return false;
  }

  Member<SVGAnimatedString> in1_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_FE_MERGE_NODE_ELEMENT_H_
