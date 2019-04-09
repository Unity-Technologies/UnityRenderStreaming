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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_PATH_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_PATH_H_

#include "third_party/blink/renderer/core/css/css_path_value.h"
#include "third_party/blink/renderer/core/svg/properties/svg_property.h"
#include "third_party/blink/renderer/core/svg/svg_parsing_error.h"
#include "third_party/blink/renderer/core/svg/svg_path_byte_stream.h"

namespace blink {

class SVGPath final : public SVGPropertyBase {
 public:
  typedef void TearOffType;

  static SVGPath* Create() { return MakeGarbageCollected<SVGPath>(); }
  static SVGPath* Create(cssvalue::CSSPathValue* path_value) {
    return MakeGarbageCollected<SVGPath>(path_value);
  }

  SVGPath();
  explicit SVGPath(cssvalue::CSSPathValue*);
  ~SVGPath() override;

  const SVGPathByteStream& ByteStream() const {
    return path_value_->ByteStream();
  }
  StylePath* GetStylePath() const { return path_value_->GetStylePath(); }
  cssvalue::CSSPathValue* PathValue() const { return path_value_.Get(); }

  // SVGPropertyBase:
  SVGPath* Clone() const;
  SVGPropertyBase* CloneForAnimation(const String&) const override;
  String ValueAsString() const override;
  SVGParsingError SetValueAsString(const String&);

  void Add(SVGPropertyBase*, SVGElement*) override;
  void CalculateAnimatedValue(SVGAnimationElement*,
                              float percentage,
                              unsigned repeat_count,
                              SVGPropertyBase* from_value,
                              SVGPropertyBase* to_value,
                              SVGPropertyBase* to_at_end_of_duration_value,
                              SVGElement*) override;
  float CalculateDistance(SVGPropertyBase* to, SVGElement*) override;

  static AnimatedPropertyType ClassType() { return kAnimatedPath; }
  AnimatedPropertyType GetType() const override { return ClassType(); }

  void Trace(blink::Visitor*) override;

 private:
  Member<cssvalue::CSSPathValue> path_value_;
};

DEFINE_SVG_PROPERTY_TYPE_CASTS(SVGPath);

}  // namespace blink

#endif
