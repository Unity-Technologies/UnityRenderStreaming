// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_PAINT_HIT_TEST_DISPLAY_ITEM_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_PAINT_HIT_TEST_DISPLAY_ITEM_H_

#include "third_party/blink/renderer/platform/graphics/paint/display_item.h"

#include "third_party/blink/renderer/platform/graphics/hit_test_rect.h"

namespace blink {

// A special DrawingDisplayItem containing hit test data.
class PLATFORM_EXPORT HitTestDisplayItem final : public DisplayItem {
 public:
  HitTestDisplayItem(const DisplayItemClient& client,
                     const HitTestRect& hit_test_rect)
      : DisplayItem(client, kHitTest, sizeof(*this)),
        hit_test_rect_(hit_test_rect) {
    DCHECK(RuntimeEnabledFeatures::PaintTouchActionRectsEnabled());
  }

  const HitTestRect& GetHitTestRect() const { return hit_test_rect_; }

  static void Record(GraphicsContext&,
                     const DisplayItemClient&,
                     const HitTestRect&);

  bool Equals(const DisplayItem& other) const final;

 private:
#if DCHECK_IS_ON()
  void PropertiesAsJSON(JSONObject&) const override;
#endif

  HitTestRect hit_test_rect_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_PAINT_HIT_TEST_DISPLAY_ITEM_H_
