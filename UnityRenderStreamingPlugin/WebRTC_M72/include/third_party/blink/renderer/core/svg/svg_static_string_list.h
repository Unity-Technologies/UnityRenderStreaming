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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_STATIC_STRING_LIST_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_STATIC_STRING_LIST_H_

#include "third_party/blink/renderer/core/svg/properties/svg_animated_property.h"
#include "third_party/blink/renderer/core/svg/svg_string_list_tear_off.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class SVGElement;

// SVGStringList property implementations for SVGTests properties.
// Inherits SVGAnimatedPropertyBase to enable XML attribute synchronization, but
// this is never animated.
class SVGStaticStringList final
    : public GarbageCollectedFinalized<SVGStaticStringList>,
      public SVGAnimatedPropertyBase {
  USING_GARBAGE_COLLECTED_MIXIN(SVGStaticStringList);

 public:
  template <char list_delimiter>
  static SVGStaticStringList* Create(SVGElement* context_element,
                                     const QualifiedName& attribute_name) {
    return MakeGarbageCollected<SVGStaticStringList>(
        context_element, attribute_name,
        SVGStringList<list_delimiter>::Create());
  }

  SVGStaticStringList(SVGElement*,
                      const QualifiedName&,
                      SVGStringListBase* initial_value);
  ~SVGStaticStringList() override;

  // SVGAnimatedPropertyBase:
  SVGPropertyBase* CurrentValueBase() override;
  const SVGPropertyBase& BaseValueBase() const override;
  bool IsAnimating() const override;
  SVGPropertyBase* CreateAnimatedValue() override;
  void SetAnimatedValue(SVGPropertyBase*) override;
  void AnimationEnded() override;

  SVGParsingError AttributeChanged(const String&) override;

  SVGStringListBase* Value() { return value_.Get(); }
  SVGStringListTearOff* TearOff();

  void Trace(blink::Visitor*) override;

 private:
  Member<SVGStringListBase> value_;
  Member<SVGStringListTearOff> tear_off_;
};

}  // namespace blink

#endif
