// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_PAINT_HIT_TEST_DATA_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_PAINT_HIT_TEST_DATA_H_

#include "third_party/blink/renderer/platform/graphics/hit_test_rect.h"
#include "third_party/blink/renderer/platform/platform_export.h"

namespace blink {

using HitTestRects = Vector<HitTestRect>;

struct PLATFORM_EXPORT HitTestData {
  HitTestRects touch_action_rects;
  HitTestRects wheel_event_handler_region;
  HitTestRects non_fast_scrollable_region;

  HitTestData() = default;
  HitTestData(const HitTestData& other)
      : touch_action_rects(other.touch_action_rects),
        wheel_event_handler_region(other.wheel_event_handler_region),
        non_fast_scrollable_region(other.non_fast_scrollable_region) {}

  bool operator==(const HitTestData& rhs) const {
    return touch_action_rects == rhs.touch_action_rects &&
           wheel_event_handler_region == rhs.wheel_event_handler_region &&
           non_fast_scrollable_region == rhs.non_fast_scrollable_region;
  }

  void Append(const HitTestRect& rect) {
    // TODO(836905): Support other types of hit testing.
    touch_action_rects.push_back(rect);
  }

  bool operator!=(const HitTestData& rhs) const { return !(*this == rhs); }

  String ToString() const;
};

PLATFORM_EXPORT std::ostream& operator<<(std::ostream&, const HitTestData&);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_PAINT_HIT_TEST_DATA_H_
