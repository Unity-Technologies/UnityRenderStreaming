/*
 * Copyright (c) 2012, Google Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_GEOMETRY_LAYOUT_UNIT_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_GEOMETRY_LAYOUT_UNIT_H_

#include <iosfwd>
#include <limits>
#include "base/compiler_specific.h"
#include "base/numerics/safe_conversions.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/assertions.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/saturated_arithmetic.h"

namespace blink {

#if DCHECK_IS_ON()
#define REPORT_OVERFLOW(doesOverflow)                                          \
  DLOG_IF(ERROR, !(doesOverflow)) << "LayoutUnit overflow !(" << #doesOverflow \
                                  << ") in " << PRETTY_FUNCTION
#else
#define REPORT_OVERFLOW(doesOverflow) ((void)0)
#endif

static const unsigned kLayoutUnitFractionalBits = 6;
static const int kFixedPointDenominator = 1 << kLayoutUnitFractionalBits;

const int kIntMaxForLayoutUnit = INT_MAX / kFixedPointDenominator;
const int kIntMinForLayoutUnit = INT_MIN / kFixedPointDenominator;

// TODO(thakis): Remove these two lines once http://llvm.org/PR26504 is resolved
class PLATFORM_EXPORT LayoutUnit;
constexpr inline bool operator<(const LayoutUnit&, const LayoutUnit&);

class LayoutUnit {
  DISALLOW_NEW();

 public:
  constexpr LayoutUnit() : value_(0) {}
  explicit LayoutUnit(int value) { SetValue(value); }
  explicit LayoutUnit(unsigned short value) { SetValue(value); }
  explicit LayoutUnit(unsigned value) { SetValue(value); }
  explicit LayoutUnit(unsigned long value) {
    value_ = base::saturated_cast<int>(value * kFixedPointDenominator);
  }
  explicit LayoutUnit(unsigned long long value) {
    value_ = base::saturated_cast<int>(value * kFixedPointDenominator);
  }
  explicit LayoutUnit(float value) {
    value_ = base::saturated_cast<int>(value * kFixedPointDenominator);
  }
  explicit LayoutUnit(double value) {
    value_ = base::saturated_cast<int>(value * kFixedPointDenominator);
  }

  static LayoutUnit FromFloatCeil(float value) {
    LayoutUnit v;
    v.value_ = base::saturated_cast<int>(ceilf(value * kFixedPointDenominator));
    return v;
  }

  static LayoutUnit FromFloatFloor(float value) {
    LayoutUnit v;
    v.value_ =
        base::saturated_cast<int>(floorf(value * kFixedPointDenominator));
    return v;
  }

  static LayoutUnit FromFloatRound(float value) {
    LayoutUnit v;
    v.value_ =
        base::saturated_cast<int>(roundf(value * kFixedPointDenominator));
    return v;
  }

  static LayoutUnit FromDoubleRound(double value) {
    LayoutUnit v;
    v.value_ = base::saturated_cast<int>(round(value * kFixedPointDenominator));
    return v;
  }

  constexpr int ToInt() const { return value_ / kFixedPointDenominator; }
  constexpr float ToFloat() const {
    return static_cast<float>(value_) / kFixedPointDenominator;
  }
  constexpr double ToDouble() const {
    return static_cast<double>(value_) / kFixedPointDenominator;
  }
  unsigned ToUnsigned() const {
    REPORT_OVERFLOW(value_ >= 0);
    return ToInt();
  }

  // Conversion to int or unsigned is lossy. 'explicit' on these operators won't
  // work because there are also other implicit conversion paths (e.g. operator
  // bool then to int which would generate wrong result). Use toInt() and
  // toUnsigned() instead.
  operator int() const = delete;
  operator unsigned() const = delete;

  constexpr operator double() const { return ToDouble(); }
  constexpr operator float() const { return ToFloat(); }
  constexpr operator bool() const { return value_; }

  LayoutUnit operator++(int) {
    value_ = ClampAdd(value_, kFixedPointDenominator);
    return *this;
  }

  constexpr int RawValue() const { return value_; }
  inline void SetRawValue(int value) { value_ = value; }
  void SetRawValue(long long value) {
    REPORT_OVERFLOW(value > std::numeric_limits<int>::min() &&
                    value < std::numeric_limits<int>::max());
    value_ = static_cast<int>(value);
  }

  LayoutUnit Abs() const {
    LayoutUnit return_value;
    return_value.SetRawValue(::abs(value_));
    return return_value;
  }
  int Ceil() const {
    if (UNLIKELY(value_ >= INT_MAX - kFixedPointDenominator + 1))
      return kIntMaxForLayoutUnit;

    if (value_ >= 0)
      return (value_ + kFixedPointDenominator - 1) / kFixedPointDenominator;
    return ToInt();
  }
  ALWAYS_INLINE int Round() const {
    return ToInt() + ((Fraction().RawValue() + (kFixedPointDenominator / 2)) >>
                      kLayoutUnitFractionalBits);
  }

  int Floor() const {
    if (UNLIKELY(value_ <= INT_MIN + kFixedPointDenominator - 1))
      return kIntMinForLayoutUnit;

    return value_ >> kLayoutUnitFractionalBits;
  }

  LayoutUnit ClampNegativeToZero() const {
    return value_ < 0 ? LayoutUnit() : *this;
  }

  LayoutUnit ClampPositiveToZero() const {
    return value_ > 0 ? LayoutUnit() : *this;
  }

  LayoutUnit Fraction() const {
    // Add the fraction to the size (as opposed to the full location) to avoid
    // overflows.  Compute fraction using the mod operator to preserve the sign
    // of the value as it may affect rounding.
    LayoutUnit fraction;
    fraction.SetRawValue(RawValue() % kFixedPointDenominator);
    return fraction;
  }

  bool MightBeSaturated() const {
    return RawValue() == std::numeric_limits<int>::max() ||
           RawValue() == std::numeric_limits<int>::min();
  }

  static float Epsilon() { return 1.0f / kFixedPointDenominator; }

  LayoutUnit AddEpsilon() const {
    LayoutUnit return_value;
    return_value.SetRawValue(
        value_ < std::numeric_limits<int>::max() ? value_ + 1 : value_);
    return return_value;
  }

  static constexpr LayoutUnit Max() {
    LayoutUnit m;
    m.value_ = std::numeric_limits<int>::max();
    return m;
  }
  static constexpr LayoutUnit Min() {
    LayoutUnit m;
    m.value_ = std::numeric_limits<int>::min();
    return m;
  }

  // Versions of max/min that are slightly smaller/larger than max/min() to
  // allow for roinding without overflowing.
  static const LayoutUnit NearlyMax() {
    LayoutUnit m;
    m.value_ = std::numeric_limits<int>::max() - kFixedPointDenominator / 2;
    return m;
  }
  static const LayoutUnit NearlyMin() {
    LayoutUnit m;
    m.value_ = std::numeric_limits<int>::min() + kFixedPointDenominator / 2;
    return m;
  }

  static LayoutUnit Clamp(double value) { return FromFloatFloor(value); }

  String ToString() const;

 private:
  static bool IsInBounds(int value) {
    return ::abs(value) <=
           std::numeric_limits<int>::max() / kFixedPointDenominator;
  }
  static bool IsInBounds(unsigned value) {
    return value <= static_cast<unsigned>(std::numeric_limits<int>::max()) /
                        kFixedPointDenominator;
  }
  static bool IsInBounds(double value) {
    return ::fabs(value) <=
           std::numeric_limits<int>::max() / kFixedPointDenominator;
  }

  ALWAYS_INLINE void SetValue(int value) {
    value_ = SaturatedSet<kLayoutUnitFractionalBits>(value);
  }

  inline void SetValue(unsigned value) {
    value_ = SaturatedSet<kLayoutUnitFractionalBits>(value);
  }

  int value_;
};

constexpr bool operator<=(const LayoutUnit& a, const LayoutUnit& b) {
  return a.RawValue() <= b.RawValue();
}

constexpr bool operator<=(const LayoutUnit& a, float b) {
  return a.ToFloat() <= b;
}

inline bool operator<=(const LayoutUnit& a, int b) {
  return a <= LayoutUnit(b);
}

constexpr bool operator<=(const float a, const LayoutUnit& b) {
  return a <= b.ToFloat();
}

inline bool operator<=(const int a, const LayoutUnit& b) {
  return LayoutUnit(a) <= b;
}

constexpr bool operator>=(const LayoutUnit& a, const LayoutUnit& b) {
  return a.RawValue() >= b.RawValue();
}

inline bool operator>=(const LayoutUnit& a, int b) {
  return a >= LayoutUnit(b);
}

constexpr bool operator>=(const float a, const LayoutUnit& b) {
  return a >= b.ToFloat();
}

constexpr bool operator>=(const LayoutUnit& a, float b) {
  return a.ToFloat() >= b;
}

inline bool operator>=(const int a, const LayoutUnit& b) {
  return LayoutUnit(a) >= b;
}

constexpr bool operator<(const LayoutUnit& a, const LayoutUnit& b) {
  return a.RawValue() < b.RawValue();
}

inline bool operator<(const LayoutUnit& a, int b) {
  return a < LayoutUnit(b);
}

constexpr bool operator<(const LayoutUnit& a, float b) {
  return a.ToFloat() < b;
}

constexpr bool operator<(const LayoutUnit& a, double b) {
  return a.ToDouble() < b;
}

inline bool operator<(const int a, const LayoutUnit& b) {
  return LayoutUnit(a) < b;
}

constexpr bool operator<(const float a, const LayoutUnit& b) {
  return a < b.ToFloat();
}

constexpr bool operator>(const LayoutUnit& a, const LayoutUnit& b) {
  return a.RawValue() > b.RawValue();
}

constexpr bool operator>(const LayoutUnit& a, double b) {
  return a.ToDouble() > b;
}

constexpr bool operator>(const LayoutUnit& a, float b) {
  return a.ToFloat() > b;
}

inline bool operator>(const LayoutUnit& a, int b) {
  return a > LayoutUnit(b);
}

inline bool operator>(const int a, const LayoutUnit& b) {
  return LayoutUnit(a) > b;
}

constexpr bool operator>(const float a, const LayoutUnit& b) {
  return a > b.ToFloat();
}

constexpr bool operator>(const double a, const LayoutUnit& b) {
  return a > b.ToDouble();
}

constexpr bool operator!=(const LayoutUnit& a, const LayoutUnit& b) {
  return a.RawValue() != b.RawValue();
}

inline bool operator!=(const LayoutUnit& a, float b) {
  return a != LayoutUnit(b);
}

inline bool operator!=(const int a, const LayoutUnit& b) {
  return LayoutUnit(a) != b;
}

inline bool operator!=(const LayoutUnit& a, int b) {
  return a != LayoutUnit(b);
}

constexpr bool operator==(const LayoutUnit& a, const LayoutUnit& b) {
  return a.RawValue() == b.RawValue();
}

inline bool operator==(const LayoutUnit& a, int b) {
  return a == LayoutUnit(b);
}

inline bool operator==(const int a, const LayoutUnit& b) {
  return LayoutUnit(a) == b;
}

constexpr bool operator==(const LayoutUnit& a, float b) {
  return a.ToFloat() == b;
}

constexpr bool operator==(const float a, const LayoutUnit& b) {
  return a == b.ToFloat();
}

// For multiplication that's prone to overflow, this bounds it to
// LayoutUnit::max() and ::min()
inline LayoutUnit BoundedMultiply(const LayoutUnit& a, const LayoutUnit& b) {
  int64_t result = static_cast<int64_t>(a.RawValue()) *
                   static_cast<int64_t>(b.RawValue()) / kFixedPointDenominator;
  int32_t high = static_cast<int32_t>(result >> 32);
  int32_t low = static_cast<int32_t>(result);
  uint32_t saturated =
      (static_cast<uint32_t>(a.RawValue() ^ b.RawValue()) >> 31) +
      std::numeric_limits<int>::max();
  // If the higher 32 bits does not match the lower 32 with sign extension the
  // operation overflowed.
  if (high != low >> 31)
    result = saturated;

  LayoutUnit return_val;
  return_val.SetRawValue(static_cast<int>(result));
  return return_val;
}

inline LayoutUnit operator*(const LayoutUnit& a, const LayoutUnit& b) {
  return BoundedMultiply(a, b);
}

inline double operator*(const LayoutUnit& a, double b) {
  return a.ToDouble() * b;
}

inline float operator*(const LayoutUnit& a, float b) {
  return a.ToFloat() * b;
}

inline LayoutUnit operator*(const LayoutUnit& a, int b) {
  return a * LayoutUnit(b);
}

inline LayoutUnit operator*(const LayoutUnit& a, unsigned short b) {
  return a * LayoutUnit(b);
}

inline LayoutUnit operator*(const LayoutUnit& a, unsigned b) {
  return a * LayoutUnit(b);
}

inline LayoutUnit operator*(const LayoutUnit& a, unsigned long b) {
  return a * LayoutUnit(b);
}

inline LayoutUnit operator*(const LayoutUnit& a, unsigned long long b) {
  return a * LayoutUnit(b);
}

inline LayoutUnit operator*(unsigned short a, const LayoutUnit& b) {
  return LayoutUnit(a) * b;
}

inline LayoutUnit operator*(unsigned a, const LayoutUnit& b) {
  return LayoutUnit(a) * b;
}

inline LayoutUnit operator*(unsigned long a, const LayoutUnit& b) {
  return LayoutUnit(a) * b;
}

inline LayoutUnit operator*(unsigned long long a, const LayoutUnit& b) {
  return LayoutUnit(a) * b;
}

inline LayoutUnit operator*(const int a, const LayoutUnit& b) {
  return LayoutUnit(a) * b;
}

constexpr float operator*(const float a, const LayoutUnit& b) {
  return a * b.ToFloat();
}

constexpr double operator*(const double a, const LayoutUnit& b) {
  return a * b.ToDouble();
}

inline LayoutUnit operator/(const LayoutUnit& a, const LayoutUnit& b) {
  LayoutUnit return_val;
  long long raw_val = static_cast<long long>(kFixedPointDenominator) *
                      a.RawValue() / b.RawValue();
  return_val.SetRawValue(base::saturated_cast<int>(raw_val));
  return return_val;
}

constexpr float operator/(const LayoutUnit& a, float b) {
  return a.ToFloat() / b;
}

constexpr double operator/(const LayoutUnit& a, double b) {
  return a.ToDouble() / b;
}

inline LayoutUnit operator/(const LayoutUnit& a, int b) {
  return a / LayoutUnit(b);
}

inline LayoutUnit operator/(const LayoutUnit& a, unsigned short b) {
  return a / LayoutUnit(b);
}

inline LayoutUnit operator/(const LayoutUnit& a, unsigned b) {
  return a / LayoutUnit(b);
}

inline LayoutUnit operator/(const LayoutUnit& a, unsigned long b) {
  return a / LayoutUnit(b);
}

inline LayoutUnit operator/(const LayoutUnit& a, unsigned long long b) {
  return a / LayoutUnit(b);
}

constexpr float operator/(const float a, const LayoutUnit& b) {
  return a / b.ToFloat();
}

constexpr double operator/(const double a, const LayoutUnit& b) {
  return a / b.ToDouble();
}

inline LayoutUnit operator/(const int a, const LayoutUnit& b) {
  return LayoutUnit(a) / b;
}

inline LayoutUnit operator/(unsigned short a, const LayoutUnit& b) {
  return LayoutUnit(a) / b;
}

inline LayoutUnit operator/(unsigned a, const LayoutUnit& b) {
  return LayoutUnit(a) / b;
}

inline LayoutUnit operator/(unsigned long a, const LayoutUnit& b) {
  return LayoutUnit(a) / b;
}

inline LayoutUnit operator/(unsigned long long a, const LayoutUnit& b) {
  return LayoutUnit(a) / b;
}

ALWAYS_INLINE LayoutUnit operator+(const LayoutUnit& a, const LayoutUnit& b) {
  LayoutUnit return_val;
  return_val.SetRawValue(ClampAdd(a.RawValue(), b.RawValue()).RawValue());
  return return_val;
}

inline LayoutUnit operator+(const LayoutUnit& a, int b) {
  return a + LayoutUnit(b);
}

inline float operator+(const LayoutUnit& a, float b) {
  return a.ToFloat() + b;
}

inline double operator+(const LayoutUnit& a, double b) {
  return a.ToDouble() + b;
}

inline LayoutUnit operator+(const int a, const LayoutUnit& b) {
  return LayoutUnit(a) + b;
}

constexpr inline float operator+(const float a, const LayoutUnit& b) {
  return a + b.ToFloat();
}

constexpr inline double operator+(const double a, const LayoutUnit& b) {
  return a + b.ToDouble();
}

ALWAYS_INLINE LayoutUnit operator-(const LayoutUnit& a, const LayoutUnit& b) {
  LayoutUnit return_val;
  return_val.SetRawValue(ClampSub(a.RawValue(), b.RawValue()).RawValue());
  return return_val;
}

inline LayoutUnit operator-(const LayoutUnit& a, int b) {
  return a - LayoutUnit(b);
}

inline LayoutUnit operator-(const LayoutUnit& a, unsigned b) {
  return a - LayoutUnit(b);
}

constexpr float operator-(const LayoutUnit& a, float b) {
  return a.ToFloat() - b;
}

constexpr double operator-(const LayoutUnit& a, double b) {
  return a.ToDouble() - b;
}

inline LayoutUnit operator-(const int a, const LayoutUnit& b) {
  return LayoutUnit(a) - b;
}

constexpr float operator-(const float a, const LayoutUnit& b) {
  return a - b.ToFloat();
}

inline LayoutUnit operator-(const LayoutUnit& a) {
  LayoutUnit return_val;
  return_val.SetRawValue((-MakeClampedNum(a.RawValue())).RawValue());
  return return_val;
}

// Returns the remainder after a division with integer results.
// This calculates the modulo so that:
//   a = static_cast<int>(a / b) * b + IntMod(a, b).
inline LayoutUnit IntMod(const LayoutUnit& a, const LayoutUnit& b) {
  LayoutUnit return_val;
  return_val.SetRawValue(a.RawValue() % b.RawValue());
  return return_val;
}

// Returns the remainder after a division with LayoutUnit results.
// This calculates the modulo so that: a = (a / b) * b + LayoutMod(a, b).
inline LayoutUnit LayoutMod(const LayoutUnit& a, const LayoutUnit& b) {
  LayoutUnit return_val;
  long long raw_val =
      (static_cast<long long>(kFixedPointDenominator) * a.RawValue()) %
      b.RawValue();
  return_val.SetRawValue(raw_val / kFixedPointDenominator);
  return return_val;
}

inline LayoutUnit LayoutMod(const LayoutUnit& a, int b) {
  return LayoutMod(a, LayoutUnit(b));
}

inline LayoutUnit& operator+=(LayoutUnit& a, const LayoutUnit& b) {
  a.SetRawValue(ClampAdd(a.RawValue(), b.RawValue()).RawValue());
  return a;
}

inline LayoutUnit& operator+=(LayoutUnit& a, int b) {
  a = a + LayoutUnit(b);
  return a;
}

inline LayoutUnit& operator+=(LayoutUnit& a, float b) {
  a = LayoutUnit(a + b);
  return a;
}

inline float& operator+=(float& a, const LayoutUnit& b) {
  a = a + b;
  return a;
}

inline LayoutUnit& operator-=(LayoutUnit& a, int b) {
  a = a - LayoutUnit(b);
  return a;
}

inline LayoutUnit& operator-=(LayoutUnit& a, const LayoutUnit& b) {
  a.SetRawValue(ClampSub(a.RawValue(), b.RawValue()).RawValue());
  return a;
}

inline LayoutUnit& operator-=(LayoutUnit& a, float b) {
  a = LayoutUnit(a - b);
  return a;
}

inline float& operator-=(float& a, const LayoutUnit& b) {
  a = a - b;
  return a;
}

inline LayoutUnit& operator*=(LayoutUnit& a, const LayoutUnit& b) {
  a = a * b;
  return a;
}

inline LayoutUnit& operator*=(LayoutUnit& a, float b) {
  a = LayoutUnit(a * b);
  return a;
}

inline float& operator*=(float& a, const LayoutUnit& b) {
  a = a * b;
  return a;
}

inline LayoutUnit& operator/=(LayoutUnit& a, const LayoutUnit& b) {
  a = a / b;
  return a;
}

inline LayoutUnit& operator/=(LayoutUnit& a, float b) {
  a = LayoutUnit(a / b);
  return a;
}

inline float& operator/=(float& a, const LayoutUnit& b) {
  a = a / b;
  return a;
}

inline int SnapSizeToPixel(LayoutUnit size, LayoutUnit location) {
  LayoutUnit fraction = location.Fraction();
  return (fraction + size).Round() - fraction.Round();
}

inline int RoundToInt(LayoutUnit value) {
  return value.Round();
}

inline int FloorToInt(LayoutUnit value) {
  return value.Floor();
}

inline LayoutUnit AbsoluteValue(const LayoutUnit& value) {
  return value.Abs();
}

inline bool IsIntegerValue(const LayoutUnit value) {
  return value.ToInt() == value;
}

PLATFORM_EXPORT std::ostream& operator<<(std::ostream&, const LayoutUnit&);
PLATFORM_EXPORT WTF::TextStream& operator<<(WTF::TextStream&,
                                            const LayoutUnit&);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_GEOMETRY_LAYOUT_UNIT_H_
