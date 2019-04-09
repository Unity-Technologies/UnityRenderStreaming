// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_XR_XR_RAY_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_XR_XR_RAY_H_

#include "third_party/blink/renderer/core/geometry/dom_point_read_only.h"
#include "third_party/blink/renderer/core/typed_arrays/dom_typed_array.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class TransformationMatrix;

class XRRay final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  explicit XRRay(std::unique_ptr<TransformationMatrix>);
  ~XRRay() override;

  DOMPointReadOnly* origin() const { return origin_; }
  DOMPointReadOnly* direction() const { return direction_; }
  DOMFloat32Array* transformMatrix() const;

  void Trace(blink::Visitor*) override;

 private:
  Member<DOMPointReadOnly> origin_;
  Member<DOMPointReadOnly> direction_;
  const std::unique_ptr<TransformationMatrix> transform_matrix_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_XR_XR_RAY_H_
