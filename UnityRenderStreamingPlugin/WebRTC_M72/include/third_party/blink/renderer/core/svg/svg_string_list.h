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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_STRING_LIST_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_STRING_LIST_H_

#include "third_party/blink/renderer/core/svg/properties/svg_property_helper.h"
#include "third_party/blink/renderer/core/svg/svg_parsing_error.h"
#include "third_party/blink/renderer/core/svg/svg_string.h"

namespace blink {

class ExceptionState;
class SVGStringListTearOff;

// Implementation of SVGStringList spec:
// http://www.w3.org/TR/SVG/single-page.html#types-InterfaceSVGStringList
// See SVGStringListTearOff for actual Javascript interface.
// Unlike other SVG*List implementations, SVGStringList is NOT tied to
// SVGString.
// SVGStringList operates directly on DOMString.
//
// In short:
//   SVGStringList has_a Vector<String>.
//   SVGStringList items are exposed to Javascript as DOMString (not SVGString)
//   as in the spec.
//   SVGString is used only for boxing values for non-list string property
//   SVGAnimatedString,
//   and not used for SVGStringList.
class SVGStringListBase : public SVGPropertyBase {
 public:
  typedef SVGStringListTearOff TearOffType;

  ~SVGStringListBase() override;

  const Vector<String>& Values() const { return values_; }

  // SVGStringList DOM Spec implementation. These are only to be called from
  // SVGStringListTearOff:
  uint32_t length() { return values_.size(); }
  void clear() { values_.clear(); }
  void Initialize(const String&);
  String GetItem(uint32_t, ExceptionState&);
  void InsertItemBefore(const String&, uint32_t);
  String RemoveItem(uint32_t, ExceptionState&);
  void AppendItem(const String&);
  void ReplaceItem(const String&, uint32_t, ExceptionState&);

  // SVGPropertyBase:
  virtual SVGParsingError SetValueAsString(const String&) = 0;

  void Add(SVGPropertyBase*, SVGElement*) override;
  void CalculateAnimatedValue(SVGAnimationElement*,
                              float percentage,
                              unsigned repeat_count,
                              SVGPropertyBase* from_value,
                              SVGPropertyBase* to_value,
                              SVGPropertyBase* to_at_end_of_duration_value,
                              SVGElement*) override;
  float CalculateDistance(SVGPropertyBase* to, SVGElement*) override;

  static AnimatedPropertyType ClassType() { return kAnimatedStringList; }

  SVGPropertyBase* CloneForAnimation(const String& value) const override {
    NOTREACHED();
    return nullptr;
  }

  AnimatedPropertyType GetType() const override { return ClassType(); }

 protected:
  SVGParsingError SetValueAsStringWithDelimiter(const String& data,
                                                char list_delimiter);
  String ValueAsStringWithDelimiter(char list_delimiter) const;

  template <typename CharType>
  void ParseInternal(const CharType*& ptr,
                     const CharType* end,
                     char list_delimiter);
  bool CheckIndexBound(uint32_t, ExceptionState&);

  Vector<String> values_;
};

template <char list_delimiter>
class SVGStringList final : public SVGStringListBase {
 public:
  static SVGStringList<list_delimiter>* Create() {
    return new SVGStringList<list_delimiter>();
  }
  ~SVGStringList() override = default;

  SVGParsingError SetValueAsString(const String& data) override {
    return SVGStringListBase::SetValueAsStringWithDelimiter(data,
                                                            list_delimiter);
  }

  String ValueAsString() const override {
    return SVGStringListBase::ValueAsStringWithDelimiter(list_delimiter);
  }

 private:
  SVGStringList() = default;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_STRING_LIST_H_
