/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_ANIMATED_LENGTH_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_ANIMATED_LENGTH_H_

#include "third_party/blink/renderer/core/svg/properties/svg_animated_property.h"
#include "third_party/blink/renderer/core/svg/svg_length_tear_off.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"

namespace blink {

class SVGAnimatedLength : public ScriptWrappable,
                          public SVGAnimatedProperty<SVGLength> {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(SVGAnimatedLength);

 public:
  static SVGAnimatedLength* Create(
      SVGElement* context_element,
      const QualifiedName& attribute_name,
      SVGLengthMode mode,
      SVGLength::Initial initial_value,
      CSSPropertyID css_property_id = CSSPropertyInvalid) {
    return MakeGarbageCollected<SVGAnimatedLength>(
        context_element, attribute_name, mode, initial_value, css_property_id);
  }

  SVGAnimatedLength(SVGElement* context_element,
                    const QualifiedName& attribute_name,
                    SVGLengthMode mode,
                    SVGLength::Initial initial_value,
                    CSSPropertyID css_property_id = CSSPropertyInvalid)
      : SVGAnimatedProperty<SVGLength>(context_element,
                                       attribute_name,
                                       SVGLength::Create(initial_value, mode),
                                       css_property_id,
                                       static_cast<unsigned>(initial_value)) {}

  SVGParsingError AttributeChanged(const String&) override;

  const CSSValue& CssValue() const {
    return CurrentValue()->AsCSSPrimitiveValue();
  }

  void Trace(blink::Visitor*) override;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_ANIMATED_LENGTH_H_
