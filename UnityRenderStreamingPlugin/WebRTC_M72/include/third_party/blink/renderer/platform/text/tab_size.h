// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_TEXT_TAB_SIZE_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_TEXT_TAB_SIZE_H_

#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace blink {

struct TabSize {
  DISALLOW_NEW();
  TabSize(float pixels) : float_value_(pixels), is_spaces_(0) {}

  TabSize(int spaces) : float_value_(spaces), is_spaces_(1) {}

  bool IsSpaces() const { return is_spaces_; }

  float GetPixelSize(float space_width) const {
    return is_spaces_ ? float_value_ * space_width : float_value_;
  }

  float float_value_;
  unsigned is_spaces_ : 1;
};

inline bool operator==(const TabSize& a, const TabSize& b) {
  return (a.float_value_ == b.float_value_) && (a.is_spaces_ == b.is_spaces_);
}

inline bool operator!=(const TabSize& a, const TabSize& b) {
  return !(a == b);
}

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_TEXT_TAB_SIZE_H_
