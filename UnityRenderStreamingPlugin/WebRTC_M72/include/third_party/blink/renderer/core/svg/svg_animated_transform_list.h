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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_ANIMATED_TRANSFORM_LIST_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_ANIMATED_TRANSFORM_LIST_H_

#include "third_party/blink/renderer/core/svg/properties/svg_animated_property.h"
#include "third_party/blink/renderer/core/svg/svg_transform_list_tear_off.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"

namespace blink {

// SVG Spec:
// http://www.w3.org/TR/SVG11/coords.html#InterfaceSVGAnimatedTransformList
class SVGAnimatedTransformList final
    : public ScriptWrappable,
      public SVGAnimatedProperty<SVGTransformList> {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(SVGAnimatedTransformList);

 public:
  static SVGAnimatedTransformList* Create(
      SVGElement* context_element,
      const QualifiedName& attribute_name,
      CSSPropertyID css_property_id = CSSPropertyInvalid) {
    return MakeGarbageCollected<SVGAnimatedTransformList>(
        context_element, attribute_name, css_property_id);
  }

  SVGAnimatedTransformList(SVGElement* context_element,
                           const QualifiedName& attribute_name,
                           CSSPropertyID css_property_id)
      : SVGAnimatedProperty<SVGTransformList>(context_element,
                                              attribute_name,
                                              SVGTransformList::Create(),
                                              css_property_id) {}

  void Trace(blink::Visitor* visitor) override {
    SVGAnimatedProperty<SVGTransformList>::Trace(visitor);
    ScriptWrappable::Trace(visitor);
  }
};

}  // namespace blink

#endif
