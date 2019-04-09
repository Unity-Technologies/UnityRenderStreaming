// Copyright (c) 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCROLL_OVERSCROLL_BEHAVIOR_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCROLL_OVERSCROLL_BEHAVIOR_H_

#include "cc/input/overscroll_behavior.h"

namespace blink {

// A wrapper around cc's structure to expose it to core.
struct OverscrollBehavior : public cc::OverscrollBehavior {
  OverscrollBehavior() = default;
  explicit OverscrollBehavior(OverscrollBehaviorType type)
      : cc::OverscrollBehavior(type) {}
  OverscrollBehavior(OverscrollBehaviorType x_type,
                     OverscrollBehaviorType y_type)
      : cc::OverscrollBehavior(x_type, y_type) {}
};

}  // namespace blink

#endif
