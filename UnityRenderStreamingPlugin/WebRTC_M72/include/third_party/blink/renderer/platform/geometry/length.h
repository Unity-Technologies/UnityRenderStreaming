/*
    Copyright (C) 1999 Lars Knoll (knoll@kde.org)
    Copyright (C) 2006, 2008 Apple Inc. All rights reserved.
    Copyright (C) 2011 Rik Cabanier (cabanier@adobe.com)
    Copyright (C) 2011 Adobe Systems Incorporated. All rights reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_GEOMETRY_LENGTH_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_GEOMETRY_LENGTH_H_

#include "third_party/blink/renderer/platform/geometry/layout_unit.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace blink {

// FIXME: This enum makes it hard to tell in general what values may be
// appropriate for any given Length.
enum LengthType {
  kAuto,
  kPercent,
  kFixed,
  kMinContent,
  kMaxContent,
  kFillAvailable,
  kFitContent,
  kCalculated,
  kExtendToZoom,
  kDeviceWidth,
  kDeviceHeight,
  kMaxSizeNone
};

enum ValueRange { kValueRangeAll, kValueRangeNonNegative };

struct PixelsAndPercent {
  DISALLOW_NEW();
  PixelsAndPercent(float pixels, float percent)
      : pixels(pixels), percent(percent) {}
  float pixels;
  float percent;
};

class CalculationValue;

class PLATFORM_EXPORT Length {
  DISALLOW_NEW();

 public:
  Length() : int_value_(0), quirk_(false), type_(kAuto), is_float_(false) {}

  explicit Length(LengthType t)
      : int_value_(0), quirk_(false), type_(t), is_float_(false) {
    DCHECK_NE(t, kCalculated);
  }

  Length(int v, LengthType t, bool q = false)
      : int_value_(v), quirk_(q), type_(t), is_float_(false) {
    DCHECK_NE(t, kCalculated);
  }

  Length(LayoutUnit v, LengthType t, bool q = false)
      : float_value_(v.ToFloat()), quirk_(q), type_(t), is_float_(true) {
    DCHECK_NE(t, kCalculated);
  }

  Length(float v, LengthType t, bool q = false)
      : float_value_(v), quirk_(q), type_(t), is_float_(true) {
    DCHECK_NE(t, kCalculated);
  }

  Length(double v, LengthType t, bool q = false)
      : quirk_(q), type_(t), is_float_(true) {
    float_value_ = static_cast<float>(v);
  }

  explicit Length(scoped_refptr<CalculationValue>);

  Length(const Length& length) {
    memcpy(this, &length, sizeof(Length));
    if (IsCalculated())
      IncrementCalculatedRef();
  }

  Length& operator=(const Length& length) {
    if (length.IsCalculated())
      length.IncrementCalculatedRef();
    if (IsCalculated())
      DecrementCalculatedRef();
    memcpy(this, &length, sizeof(Length));
    return *this;
  }

  ~Length() {
    if (IsCalculated())
      DecrementCalculatedRef();
  }

  bool operator==(const Length& o) const {
    return (type_ == o.type_) && (quirk_ == o.quirk_) &&
           (IsMaxSizeNone() || (GetFloatValue() == o.GetFloatValue()) ||
            IsCalculatedEqual(o));
  }
  bool operator!=(const Length& o) const { return !(*this == o); }

  const Length& operator*=(float v) {
    if (IsCalculated()) {
      NOTREACHED();
      return *this;
    }

    if (is_float_)
      float_value_ = static_cast<float>(float_value_ * v);
    else
      int_value_ = static_cast<int>(int_value_ * v);

    return *this;
  }

  // FIXME: Make this private (if possible) or at least rename it
  // (http://crbug.com/432707).
  inline float Value() const {
    DCHECK(!IsCalculated());
    return GetFloatValue();
  }

  int IntValue() const {
    if (IsCalculated()) {
      NOTREACHED();
      return 0;
    }
    return GetIntValue();
  }

  float Pixels() const {
    DCHECK_EQ(GetType(), kFixed);
    return GetFloatValue();
  }

  float Percent() const {
    DCHECK_EQ(GetType(), kPercent);
    return GetFloatValue();
  }

  PixelsAndPercent GetPixelsAndPercent() const;

  CalculationValue& GetCalculationValue() const;

  LengthType GetType() const { return static_cast<LengthType>(type_); }
  bool Quirk() const { return quirk_; }

  void SetQuirk(bool quirk) { quirk_ = quirk; }

  void SetValue(LengthType t, int value) {
    type_ = t;
    int_value_ = value;
    is_float_ = false;
  }

  void SetValue(int value) {
    if (IsCalculated()) {
      NOTREACHED();
      return;
    }
    SetValue(kFixed, value);
  }

  void SetValue(LengthType t, float value) {
    type_ = t;
    float_value_ = value;
    is_float_ = true;
  }

  void SetValue(LengthType t, LayoutUnit value) {
    type_ = t;
    float_value_ = value.ToFloat();
    is_float_ = true;
  }

  void SetValue(float value) { *this = Length(value, kFixed); }

  bool IsMaxSizeNone() const { return GetType() == kMaxSizeNone; }

  // FIXME calc: https://bugs.webkit.org/show_bug.cgi?id=80357. A calculated
  // Length always contains a percentage, and without a maxValue passed to these
  // functions it's impossible to determine the sign or zero-ness. We assume all
  // calc values are positive and non-zero for now.
  bool IsZero() const {
    DCHECK(!IsMaxSizeNone());
    if (IsCalculated())
      return false;

    return is_float_ ? !float_value_ : !int_value_;
  }
  bool IsPositive() const {
    if (IsMaxSizeNone())
      return false;
    if (IsCalculated())
      return true;

    return GetFloatValue() > 0;
  }
  bool IsNegative() const {
    if (IsMaxSizeNone() || IsCalculated())
      return false;

    return GetFloatValue() < 0;
  }

  bool IsAuto() const { return GetType() == kAuto; }
  bool IsFixed() const { return GetType() == kFixed; }
  bool IsIntrinsicOrAuto() const { return GetType() == kAuto || IsIntrinsic(); }
  bool IsIntrinsic() const {
    return GetType() == kMinContent || GetType() == kMaxContent ||
           GetType() == kFillAvailable || GetType() == kFitContent;
  }
  bool IsSpecified() const {
    return GetType() == kFixed || GetType() == kPercent ||
           GetType() == kCalculated;
  }
  bool IsSpecifiedOrIntrinsic() const { return IsSpecified() || IsIntrinsic(); }
  bool IsCalculated() const { return GetType() == kCalculated; }
  bool IsCalculatedEqual(const Length&) const;
  bool IsMinContent() const { return GetType() == kMinContent; }
  bool IsMaxContent() const { return GetType() == kMaxContent; }
  bool IsFillAvailable() const { return GetType() == kFillAvailable; }
  bool IsFitContent() const { return GetType() == kFitContent; }
  bool IsPercent() const { return GetType() == kPercent; }
  bool IsPercentOrCalc() const {
    return GetType() == kPercent || GetType() == kCalculated;
  }

  Length Blend(const Length& from, double progress, ValueRange range) const {
    DCHECK(IsSpecified());
    DCHECK(from.IsSpecified());

    if (progress == 0.0)
      return from;

    if (progress == 1.0)
      return *this;

    if (from.GetType() == kCalculated || GetType() == kCalculated)
      return BlendMixedTypes(from, progress, range);

    if (!from.IsZero() && !IsZero() && from.GetType() != GetType())
      return BlendMixedTypes(from, progress, range);

    if (from.IsZero() && IsZero())
      return *this;

    return BlendSameTypes(from, progress, range);
  }

  float GetFloatValue() const {
    DCHECK(!IsMaxSizeNone());
    return is_float_ ? float_value_ : int_value_;
  }
  float NonNanCalculatedValue(LayoutUnit max_value) const;

  Length SubtractFromOneHundredPercent() const;

  Length Zoom(double factor) const;

 private:
  int GetIntValue() const {
    DCHECK(!IsMaxSizeNone());
    return is_float_ ? static_cast<int>(float_value_) : int_value_;
  }

  Length BlendMixedTypes(const Length& from, double progress, ValueRange) const;

  Length BlendSameTypes(const Length& from, double progress, ValueRange) const;

  int CalculationHandle() const {
    DCHECK(IsCalculated());
    return GetIntValue();
  }
  void IncrementCalculatedRef() const;
  void DecrementCalculatedRef() const;

  union {
    int int_value_;
    float float_value_;
  };
  bool quirk_;
  unsigned char type_;
  bool is_float_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_GEOMETRY_LENGTH_H_
