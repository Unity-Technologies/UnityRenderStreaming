// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_GEOMETRY_DOM_QUAD_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_GEOMETRY_DOM_QUAD_H_

#include "third_party/blink/renderer/bindings/core/v8/script_value.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"

namespace blink {

class DOMPoint;
class DOMPointInit;
class DOMQuadInit;
class DOMRect;
class DOMRectInit;

class CORE_EXPORT DOMQuad : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static DOMQuad* Create(const DOMPointInit* p1,
                         const DOMPointInit* p2,
                         const DOMPointInit* p3,
                         const DOMPointInit* p4);
  static DOMQuad* fromRect(const DOMRectInit*);
  static DOMQuad* fromQuad(const DOMQuadInit*);

  DOMQuad(const DOMPointInit* p1,
          const DOMPointInit* p2,
          const DOMPointInit* p3,
          const DOMPointInit* p4);
  DOMQuad(double x, double y, double width, double height);

  DOMPoint* p1() const { return p1_; }
  DOMPoint* p2() const { return p2_; }
  DOMPoint* p3() const { return p3_; }
  DOMPoint* p4() const { return p4_; }

  DOMRect* getBounds();

  ScriptValue toJSONForBinding(ScriptState*) const;

  void Trace(blink::Visitor* visitor) override {
    visitor->Trace(p1_);
    visitor->Trace(p2_);
    visitor->Trace(p3_);
    visitor->Trace(p4_);
    ScriptWrappable::Trace(visitor);
  }

 private:
  void CalculateBounds();

  Member<DOMPoint> p1_;
  Member<DOMPoint> p2_;
  Member<DOMPoint> p3_;
  Member<DOMPoint> p4_;

  double left_;
  double right_;
  double top_;
  double bottom_;
};

}  // namespace blink

#endif
