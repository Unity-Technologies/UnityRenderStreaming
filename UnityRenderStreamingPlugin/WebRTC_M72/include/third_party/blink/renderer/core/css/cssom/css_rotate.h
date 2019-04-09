// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSSOM_CSS_ROTATE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSSOM_CSS_ROTATE_H_

#include "base/macros.h"
#include "third_party/blink/renderer/core/css/cssom/css_numeric_value.h"
#include "third_party/blink/renderer/core/css/cssom/css_transform_component.h"

namespace blink {

class DOMMatrix;
class ExceptionState;
class CSSNumericValue;

// Represents a rotation value in a CSSTransformValue used for properties like
// "transform".
// See CSSRotate.idl for more information about this class.
class CORE_EXPORT CSSRotate final : public CSSTransformComponent {
  DEFINE_WRAPPERTYPEINFO();

 public:
  // Constructors defined in the IDL.
  static CSSRotate* Create(CSSNumericValue* angle, ExceptionState&);
  static CSSRotate* Create(const CSSNumberish& x,
                           const CSSNumberish& y,
                           const CSSNumberish& z,
                           CSSNumericValue* angle,
                           ExceptionState&);

  // Blink-internal ways of creating CSSRotates.
  static CSSRotate* Create(CSSNumericValue* angle);
  static CSSRotate* Create(CSSNumericValue* x,
                           CSSNumericValue* y,
                           CSSNumericValue* z,
                           CSSNumericValue* angle);
  static CSSRotate* FromCSSValue(const CSSFunctionValue&);

  CSSRotate(CSSNumericValue* x,
            CSSNumericValue* y,
            CSSNumericValue* z,
            CSSNumericValue* angle,
            bool is2D);

  // Getters and setters for attributes defined in the IDL.
  CSSNumericValue* angle() { return angle_.Get(); }
  void setAngle(CSSNumericValue* angle, ExceptionState&);
  void x(CSSNumberish& x) { x.SetCSSNumericValue(x_); }
  void y(CSSNumberish& y) { y.SetCSSNumericValue(y_); }
  void z(CSSNumberish& z) { z.SetCSSNumericValue(z_); }
  void setX(const CSSNumberish&, ExceptionState&);
  void setY(const CSSNumberish&, ExceptionState&);
  void setZ(const CSSNumberish&, ExceptionState&);

  DOMMatrix* toMatrix(ExceptionState&) const final;

  // Internal methods - from CSSTransformComponent.
  TransformComponentType GetType() const final { return kRotationType; }
  const CSSFunctionValue* ToCSSValue() const final;

  void Trace(blink::Visitor* visitor) override {
    visitor->Trace(angle_);
    visitor->Trace(x_);
    visitor->Trace(y_);
    visitor->Trace(z_);
    CSSTransformComponent::Trace(visitor);
  }

 private:
  Member<CSSNumericValue> angle_;
  Member<CSSNumericValue> x_;
  Member<CSSNumericValue> y_;
  Member<CSSNumericValue> z_;
  DISALLOW_COPY_AND_ASSIGN(CSSRotate);
};

}  // namespace blink

#endif
