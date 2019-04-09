// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_LINK_HIGHLIGHT_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_LINK_HIGHLIGHT_H_

#include "third_party/blink/renderer/platform/graphics/paint/display_item_client.h"
#include "third_party/blink/renderer/platform/platform_export.h"

namespace cc {
class Layer;
}

namespace blink {

class EffectPaintPropertyNode;

class PLATFORM_EXPORT LinkHighlight : public DisplayItemClient {
 public:
  ~LinkHighlight() override {}

  virtual void Invalidate() = 0;
  virtual void ClearCurrentGraphicsLayer() = 0;
  virtual cc::Layer* Layer() = 0;

  virtual const EffectPaintPropertyNode* effect() = 0;

  // DisplayItemClient methods
  String DebugName() const final { return "LinkHighlight"; }
  LayoutRect VisualRect() const final { return LayoutRect(); }
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_LINK_HIGHLIGHT_H_
