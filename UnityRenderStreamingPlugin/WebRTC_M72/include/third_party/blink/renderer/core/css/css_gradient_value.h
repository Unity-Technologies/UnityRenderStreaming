/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_GRADIENT_VALUE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_GRADIENT_VALUE_H_

#include "base/memory/scoped_refptr.h"
#include "third_party/blink/renderer/core/css/css_identifier_value.h"
#include "third_party/blink/renderer/core/css/css_image_generator_value.h"
#include "third_party/blink/renderer/core/css/css_primitive_value.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class Color;
class Gradient;
class Document;

namespace cssvalue {

enum CSSGradientType {
  kCSSDeprecatedLinearGradient,
  kCSSDeprecatedRadialGradient,
  kCSSPrefixedLinearGradient,
  kCSSPrefixedRadialGradient,
  kCSSLinearGradient,
  kCSSRadialGradient,
  kCSSConicGradient
};
enum CSSGradientRepeat { kNonRepeating, kRepeating };

// This struct is stack allocated and allocated as part of vectors.
// When allocated on the stack its members are found by conservative
// stack scanning. When allocated as part of Vectors in heap-allocated
// objects its members are visited via the containing object's
// (CSSGradientValue) traceAfterDispatch method.
//
// http://www.w3.org/TR/css3-images/#color-stop-syntax
struct CSSGradientColorStop {
  DISALLOW_NEW();

  bool operator==(const CSSGradientColorStop& other) const {
    return DataEquivalent(color_, other.color_) &&
           DataEquivalent(offset_, other.offset_);
  }

  bool IsHint() const {
    DCHECK(color_ || offset_);
    return !color_;
  }

  bool IsCacheable() const;

  void Trace(blink::Visitor*);

  Member<const CSSPrimitiveValue> offset_;  // percentage | length | angle
  Member<const CSSValue> color_;
};

}  // namespace cssvalue
}  // namespace blink

// We have to declare the VectorTraits specialization before CSSGradientValue
// declares its inline capacity vector below.
WTF_ALLOW_MOVE_AND_INIT_WITH_MEM_FUNCTIONS(
    blink::cssvalue::CSSGradientColorStop);

namespace blink {
namespace cssvalue {

class CSSGradientValue : public CSSImageGeneratorValue {
 public:
  scoped_refptr<Image> GetImage(const ImageResourceObserver&,
                                const Document&,
                                const ComputedStyle&,
                                const FloatSize&) const;

  void AddStop(const CSSGradientColorStop& stop) {
    stops_.push_back(stop);
    is_cacheable_ = is_cacheable_ && stop.IsCacheable();
  }

  size_t StopCount() const { return stops_.size(); }

  bool IsRepeating() const { return repeating_; }

  CSSGradientType GradientType() const { return gradient_type_; }

  bool IsFixedSize() const { return false; }
  FloatSize FixedSize(const Document&) const { return FloatSize(); }

  bool IsPending() const { return false; }
  bool KnownToBeOpaque(const Document&, const ComputedStyle&) const;

  void LoadSubimages(const Document&) {}

  Vector<Color> GetStopColors(const Document&, const ComputedStyle&) const;

  void TraceAfterDispatch(blink::Visitor*);

  struct GradientDesc;

 protected:
  CSSGradientValue(ClassType class_type,
                   CSSGradientRepeat repeat,
                   CSSGradientType gradient_type)
      : CSSImageGeneratorValue(class_type),
        gradient_type_(gradient_type),
        repeating_(repeat == kRepeating),
        is_cacheable_(true) {}

  void AddStops(GradientDesc&,
                const CSSToLengthConversionData&,
                const Document&,
                const ComputedStyle&) const;
  void AddDeprecatedStops(GradientDesc&,
                          const Document&,
                          const ComputedStyle&) const;

  void AppendCSSTextForColorStops(StringBuilder&,
                                  bool requires_separator) const;
  void AppendCSSTextForDeprecatedColorStops(StringBuilder&) const;

  // Stops
  HeapVector<CSSGradientColorStop, 2> stops_;
  CSSGradientType gradient_type_;
  bool repeating_ : 1;
  bool is_cacheable_ : 1;
};

DEFINE_CSS_VALUE_TYPE_CASTS(CSSGradientValue, IsGradientValue());

class CSSLinearGradientValue final : public CSSGradientValue {
 public:
  static CSSGradientValue* Create(
      const CSSValue* first_x,
      const CSSValue* first_y,
      const CSSValue* second_x,
      const CSSValue* second_y,
      const CSSPrimitiveValue* angle,
      CSSGradientRepeat repeat,
      CSSGradientType gradient_type = kCSSLinearGradient) {
    return new CSSLinearGradientValue(first_x, first_y, second_x, second_y,
                                      angle, repeat, gradient_type);
  }

  String CustomCSSText() const;

  // Create the gradient for a given size.
  scoped_refptr<Gradient> CreateGradient(const CSSToLengthConversionData&,
                                         const FloatSize&,
                                         const Document&,
                                         const ComputedStyle&) const;

  bool Equals(const CSSLinearGradientValue&) const;

  void TraceAfterDispatch(blink::Visitor*);

 private:
  CSSLinearGradientValue(const CSSValue* first_x,
                         const CSSValue* first_y,
                         const CSSValue* second_x,
                         const CSSValue* second_y,
                         const CSSPrimitiveValue* angle,
                         CSSGradientRepeat repeat,
                         CSSGradientType gradient_type = kCSSLinearGradient)
      : CSSGradientValue(kLinearGradientClass, repeat, gradient_type),
        first_x_(first_x),
        first_y_(first_y),
        second_x_(second_x),
        second_y_(second_y),
        angle_(angle) {}

  // Any of these may be null.
  Member<const CSSValue> first_x_;
  Member<const CSSValue> first_y_;
  Member<const CSSValue> second_x_;
  Member<const CSSValue> second_y_;
  Member<const CSSPrimitiveValue> angle_;
};

DEFINE_CSS_VALUE_TYPE_CASTS(CSSLinearGradientValue, IsLinearGradientValue());

class CSSRadialGradientValue final : public CSSGradientValue {
 public:
  static CSSGradientValue* Create(
      const CSSValue* first_x,
      const CSSValue* first_y,
      const CSSPrimitiveValue* first_radius,
      const CSSValue* second_x,
      const CSSValue* second_y,
      const CSSPrimitiveValue* second_radius,
      CSSGradientRepeat repeat,
      CSSGradientType gradient_type = kCSSRadialGradient) {
    return new CSSRadialGradientValue(first_x, first_y, first_radius, second_x,
                                      second_y, second_radius, nullptr, nullptr,
                                      nullptr, nullptr, repeat, gradient_type);
  }

  static CSSGradientValue* Create(const CSSValue* center_x,
                                  const CSSValue* center_y,
                                  const CSSIdentifierValue* shape,
                                  const CSSIdentifierValue* sizing_behavior,
                                  const CSSPrimitiveValue* horizontal_size,
                                  const CSSPrimitiveValue* vertical_size,
                                  CSSGradientRepeat repeat,
                                  CSSGradientType gradient_type) {
    return new CSSRadialGradientValue(
        center_x, center_y, nullptr, center_x, center_y, nullptr, shape,
        sizing_behavior, horizontal_size, vertical_size, repeat, gradient_type);
  }

  String CustomCSSText() const;

  void SetShape(CSSIdentifierValue* val) { shape_ = val; }
  void SetSizingBehavior(CSSIdentifierValue* val) { sizing_behavior_ = val; }

  void SetEndHorizontalSize(CSSPrimitiveValue* val) {
    end_horizontal_size_ = val;
  }
  void SetEndVerticalSize(CSSPrimitiveValue* val) { end_vertical_size_ = val; }

  // Create the gradient for a given size.
  scoped_refptr<Gradient> CreateGradient(const CSSToLengthConversionData&,
                                         const FloatSize&,
                                         const Document&,
                                         const ComputedStyle&) const;

  bool Equals(const CSSRadialGradientValue&) const;

  void TraceAfterDispatch(blink::Visitor*);

 private:
  CSSRadialGradientValue(const CSSValue* first_x,
                         const CSSValue* first_y,
                         const CSSPrimitiveValue* first_radius,
                         const CSSValue* second_x,
                         const CSSValue* second_y,
                         const CSSPrimitiveValue* second_radius,
                         const CSSIdentifierValue* shape,
                         const CSSIdentifierValue* sizing_behavior,
                         const CSSPrimitiveValue* horizontal_size,
                         const CSSPrimitiveValue* vertical_size,
                         CSSGradientRepeat repeat,
                         CSSGradientType gradient_type = kCSSRadialGradient)
      : CSSGradientValue(kRadialGradientClass, repeat, gradient_type),
        first_x_(first_x),
        first_y_(first_y),
        second_x_(second_x),
        second_y_(second_y),
        first_radius_(first_radius),
        second_radius_(second_radius),
        shape_(shape),
        sizing_behavior_(sizing_behavior),
        end_horizontal_size_(horizontal_size),
        end_vertical_size_(vertical_size) {}

  // Any of these may be null.
  Member<const CSSValue> first_x_;
  Member<const CSSValue> first_y_;
  Member<const CSSValue> second_x_;
  Member<const CSSValue> second_y_;

  // These may be null for non-deprecated gradients.
  Member<const CSSPrimitiveValue> first_radius_;
  Member<const CSSPrimitiveValue> second_radius_;

  // The below are only used for non-deprecated gradients. Any of them may be
  // null.
  Member<const CSSIdentifierValue> shape_;
  Member<const CSSIdentifierValue> sizing_behavior_;

  Member<const CSSPrimitiveValue> end_horizontal_size_;
  Member<const CSSPrimitiveValue> end_vertical_size_;
};

DEFINE_CSS_VALUE_TYPE_CASTS(CSSRadialGradientValue, IsRadialGradientValue());

class CSSConicGradientValue final : public CSSGradientValue {
 public:
  static CSSGradientValue* Create(const CSSValue* x,
                                  const CSSValue* y,
                                  const CSSPrimitiveValue* from_angle,
                                  CSSGradientRepeat repeat) {
    return new CSSConicGradientValue(x, y, from_angle, repeat);
  }

  String CustomCSSText() const;

  // Create the gradient for a given size.
  scoped_refptr<Gradient> CreateGradient(const CSSToLengthConversionData&,
                                         const FloatSize&,
                                         const Document&,
                                         const ComputedStyle&) const;

  bool Equals(const CSSConicGradientValue&) const;

  void TraceAfterDispatch(blink::Visitor*);

 private:
  CSSConicGradientValue(const CSSValue* x,
                        const CSSValue* y,
                        const CSSPrimitiveValue* from_angle,
                        CSSGradientRepeat repeat)
      : CSSGradientValue(kConicGradientClass, repeat, kCSSConicGradient),
        x_(x),
        y_(y),
        from_angle_(from_angle) {}

  // Any of these may be null.
  Member<const CSSValue> x_;
  Member<const CSSValue> y_;
  Member<const CSSPrimitiveValue> from_angle_;
};

DEFINE_CSS_VALUE_TYPE_CASTS(CSSConicGradientValue, IsConicGradientValue());

}  // namespace cssvalue
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_GRADIENT_VALUE_H_
