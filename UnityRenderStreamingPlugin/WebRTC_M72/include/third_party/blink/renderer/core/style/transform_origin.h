// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_STYLE_TRANSFORM_ORIGIN_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_STYLE_TRANSFORM_ORIGIN_H_

#include "third_party/blink/renderer/platform/geometry/length.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace blink {

class TransformOrigin {
  DISALLOW_NEW();

 public:
  TransformOrigin(const Length& x, const Length& y, float z)
      : x_(x), y_(y), z_(z) {}
  bool operator==(const TransformOrigin& o) const {
    return x_ == o.x_ && y_ == o.y_ && z_ == o.z_;
  }
  bool operator!=(const TransformOrigin& o) const { return !(*this == o); }
  const Length& X() const { return x_; }
  const Length& Y() const { return y_; }
  float Z() const { return z_; }

 private:
  Length x_;
  Length y_;
  float z_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_STYLE_TRANSFORM_ORIGIN_H_
