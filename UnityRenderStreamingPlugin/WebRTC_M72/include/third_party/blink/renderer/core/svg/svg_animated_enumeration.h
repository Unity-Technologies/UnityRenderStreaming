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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_ANIMATED_ENUMERATION_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_ANIMATED_ENUMERATION_H_

#include "third_party/blink/renderer/core/svg/svg_animated_enumeration_base.h"

namespace blink {

template <typename Enum>
class SVGAnimatedEnumeration : public SVGAnimatedEnumerationBase {
 public:
  static SVGAnimatedEnumeration<Enum>* Create(
      SVGElement* context_element,
      const QualifiedName& attribute_name,
      Enum initial_value) {
    return MakeGarbageCollected<SVGAnimatedEnumeration>(
        context_element, attribute_name,
        SVGEnumeration<Enum>::Create(initial_value),
        static_cast<unsigned>(initial_value));
  }

  static SVGAnimatedEnumeration<Enum>* Create(
      SVGElement* context_element,
      const QualifiedName& attribute_name,
      SVGEnumeration<Enum>* initial_value) {
    return MakeGarbageCollected<SVGAnimatedEnumeration>(
        context_element, attribute_name, initial_value,
        static_cast<unsigned>(initial_value->EnumValue()));
  }

  SVGAnimatedEnumeration(SVGElement* context_element,
                         const QualifiedName& attribute_name,
                         SVGEnumeration<Enum>* initial_value,
                         unsigned initial_enum_value)
      : SVGAnimatedEnumerationBase(context_element,
                                   attribute_name,
                                   initial_value,
                                   initial_enum_value) {}

  SVGEnumeration<Enum>* BaseValue() {
    return static_cast<SVGEnumeration<Enum>*>(
        SVGAnimatedEnumerationBase::BaseValue());
  }

  SVGEnumeration<Enum>* CurrentValue() {
    return static_cast<SVGEnumeration<Enum>*>(
        SVGAnimatedEnumerationBase::CurrentValue());
  }

  const SVGEnumeration<Enum>* CurrentValue() const {
    return static_cast<const SVGEnumeration<Enum>*>(
        SVGAnimatedEnumerationBase::CurrentValue());
  }
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_ANIMATED_ENUMERATION_H_
