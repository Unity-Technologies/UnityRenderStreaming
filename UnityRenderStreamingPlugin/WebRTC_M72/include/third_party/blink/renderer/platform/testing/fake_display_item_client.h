// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_FAKE_DISPLAY_ITEM_CLIENT_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_FAKE_DISPLAY_ITEM_CLIENT_H_

#include "third_party/blink/renderer/platform/geometry/layout_rect.h"
#include "third_party/blink/renderer/platform/graphics/paint/display_item_client.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

namespace blink {

// A simple DisplayItemClient implementation suitable for use in unit tests.
class FakeDisplayItemClient : public DisplayItemClient {
 public:
  FakeDisplayItemClient(const String& name = "FakeDisplayItemClient",
                        const LayoutRect& visual_rect = LayoutRect())
      : name_(name), visual_rect_(visual_rect) {}

  String DebugName() const final { return name_; }
  LayoutRect VisualRect() const override { return visual_rect_; }
  LayoutRect PartialInvalidationVisualRect() const override {
    return partial_invalidation_visual_rect_;
  }
  void ClearPartialInvalidationVisualRect() const override {
    partial_invalidation_visual_rect_ = LayoutRect();
  }

  void SetVisualRect(const LayoutRect& r) { visual_rect_ = r; }
  void SetPartialInvalidationVisualRect(const LayoutRect& r) {
    Invalidate(PaintInvalidationReason::kRectangle);
    partial_invalidation_visual_rect_ = r;
  }

  // This simulates a paint without needing a PaintController.
  using DisplayItemClient::Validate;

 private:
  String name_;
  LayoutRect visual_rect_;
  mutable LayoutRect partial_invalidation_visual_rect_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_FAKE_DISPLAY_ITEM_CLIENT_H_
