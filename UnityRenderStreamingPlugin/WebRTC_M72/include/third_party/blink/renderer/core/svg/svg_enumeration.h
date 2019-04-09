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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_ENUMERATION_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_ENUMERATION_H_

#include "third_party/blink/renderer/core/svg/properties/svg_property.h"
#include "third_party/blink/renderer/core/svg/svg_parsing_error.h"

namespace blink {

class SVGEnumerationMap;

class SVGEnumerationBase : public SVGPropertyBase {
 public:
  // SVGEnumeration does not have a tear-off type.
  typedef void TearOffType;
  typedef unsigned short PrimitiveType;

  ~SVGEnumerationBase() override;

  unsigned short Value() const {
    return value_ <= MaxExposedEnumValue() ? value_ : 0;
  }
  void SetValue(unsigned short);

  // SVGPropertyBase:
  virtual SVGEnumerationBase* Clone() const = 0;
  SVGPropertyBase* CloneForAnimation(const String&) const override;

  String ValueAsString() const override;
  SVGParsingError SetValueAsString(const String&);

  void Add(SVGPropertyBase*, SVGElement*) override;
  void CalculateAnimatedValue(SVGAnimationElement*,
                              float percentage,
                              unsigned repeat_count,
                              SVGPropertyBase* from,
                              SVGPropertyBase* to,
                              SVGPropertyBase* to_at_end_of_duration_value,
                              SVGElement*) override;
  float CalculateDistance(SVGPropertyBase* to, SVGElement*) override;

  static AnimatedPropertyType ClassType() { return kAnimatedEnumeration; }
  AnimatedPropertyType GetType() const override { return ClassType(); }

  // This is the maximum value that is exposed as an IDL constant on the
  // relevant interface.
  unsigned short MaxExposedEnumValue() const;

  void SetInitial(unsigned value) {
    SetValue(static_cast<unsigned short>(value));
  }
  static constexpr int kInitialValueBits = 3;

 protected:
  SVGEnumerationBase(unsigned short value, const SVGEnumerationMap& map)
      : value_(value), map_(map) {}

  // This is the maximum value of all the internal enumeration values.
  // This assumes that the map is sorted on the enumeration value.
  unsigned short MaxInternalEnumValue() const;

  // Used by SVGMarkerOrientEnumeration.
  virtual void NotifyChange() {}

  unsigned short value_;
  const SVGEnumerationMap& map_;
};

template <typename Enum>
const SVGEnumerationMap& GetEnumerationMap();

#define DECLARE_SVG_ENUM_MAP(cpp_enum_type) \
  template <>                               \
  const SVGEnumerationMap& GetEnumerationMap<cpp_enum_type>()

template <typename Enum>
class SVGEnumeration : public SVGEnumerationBase {
 public:
  static SVGEnumeration<Enum>* Create(Enum new_value) {
    return new SVGEnumeration<Enum>(new_value);
  }

  ~SVGEnumeration() override = default;

  SVGEnumerationBase* Clone() const override { return Create(EnumValue()); }

  Enum EnumValue() const {
    DCHECK_LE(value_, MaxInternalEnumValue());
    return static_cast<Enum>(value_);
  }

  void SetEnumValue(Enum value) {
    value_ = value;
    NotifyChange();
  }

 protected:
  explicit SVGEnumeration(Enum new_value)
      : SVGEnumerationBase(new_value, GetEnumerationMap<Enum>()) {}
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_ENUMERATION_H_
