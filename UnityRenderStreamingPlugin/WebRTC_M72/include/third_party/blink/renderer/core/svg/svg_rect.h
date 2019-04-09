/*
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_RECT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_RECT_H_

#include "third_party/blink/renderer/core/svg/properties/svg_property_helper.h"
#include "third_party/blink/renderer/core/svg/svg_parsing_error.h"
#include "third_party/blink/renderer/platform/geometry/float_rect.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace blink {

class SVGRectTearOff;

class SVGRect final : public SVGPropertyHelper<SVGRect> {
 public:
  typedef SVGRectTearOff TearOffType;

  static SVGRect* Create() { return MakeGarbageCollected<SVGRect>(); }

  static SVGRect* CreateInvalid() {
    SVGRect* rect = MakeGarbageCollected<SVGRect>();
    rect->SetInvalid();
    return rect;
  }

  static SVGRect* Create(const FloatRect& rect) {
    return MakeGarbageCollected<SVGRect>(rect);
  }

  SVGRect();
  SVGRect(const FloatRect&);

  SVGRect* Clone() const;

  const FloatRect& Value() const { return value_; }
  void SetValue(const FloatRect& v) { value_ = v; }

  float X() const { return value_.X(); }
  float Y() const { return value_.Y(); }
  float Width() const { return value_.Width(); }
  float Height() const { return value_.Height(); }
  void SetX(float f) { value_.SetX(f); }
  void SetY(float f) { value_.SetY(f); }
  void SetWidth(float f) { value_.SetWidth(f); }
  void SetHeight(float f) { value_.SetHeight(f); }

  String ValueAsString() const override;
  SVGParsingError SetValueAsString(const String&);

  void Add(SVGPropertyBase*, SVGElement*) override;
  void CalculateAnimatedValue(SVGAnimationElement*,
                              float percentage,
                              unsigned repeat_count,
                              SVGPropertyBase* from,
                              SVGPropertyBase* to,
                              SVGPropertyBase* to_at_end_of_duration_value,
                              SVGElement* context_element) override;
  float CalculateDistance(SVGPropertyBase* to,
                          SVGElement* context_element) override;

  bool IsValid() const { return is_valid_; }
  void SetInvalid();

  static AnimatedPropertyType ClassType() { return kAnimatedRect; }

 private:
  template <typename CharType>
  SVGParsingError Parse(const CharType*& ptr, const CharType* end);

  bool is_valid_;
  FloatRect value_;
};

DEFINE_SVG_PROPERTY_TYPE_CASTS(SVGRect);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_RECT_H_
