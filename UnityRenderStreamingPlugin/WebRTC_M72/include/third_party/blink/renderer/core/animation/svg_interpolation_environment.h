// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_SVG_INTERPOLATION_ENVIRONMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_SVG_INTERPOLATION_ENVIRONMENT_H_

#include "third_party/blink/renderer/core/animation/interpolation_environment.h"
#include "third_party/blink/renderer/platform/wtf/assertions.h"

namespace blink {

class SVGPropertyBase;
class SVGElement;

class SVGInterpolationEnvironment : public InterpolationEnvironment {
 public:
  explicit SVGInterpolationEnvironment(const InterpolationTypesMap& map,
                                       SVGElement& svg_element,
                                       const SVGPropertyBase& svg_base_value)
      : InterpolationEnvironment(map),
        svg_element_(&svg_element),
        svg_base_value_(&svg_base_value) {}

  bool IsSVG() const final { return true; }

  SVGElement& SvgElement() {
    DCHECK(svg_element_);
    return *svg_element_;
  }
  const SVGElement& SvgElement() const {
    DCHECK(svg_element_);
    return *svg_element_;
  }

  const SVGPropertyBase& SvgBaseValue() const {
    DCHECK(svg_base_value_);
    return *svg_base_value_;
  }

 private:
  Member<SVGElement> svg_element_ = nullptr;
  Member<const SVGPropertyBase> svg_base_value_ = nullptr;
};

DEFINE_TYPE_CASTS(SVGInterpolationEnvironment,
                  InterpolationEnvironment,
                  value,
                  value->IsSVG(),
                  value.IsSVG());

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_SVG_INTERPOLATION_ENVIRONMENT_H_
