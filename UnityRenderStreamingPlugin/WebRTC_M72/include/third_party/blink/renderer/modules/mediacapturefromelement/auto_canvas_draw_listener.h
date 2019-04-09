// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIACAPTUREFROMELEMENT_AUTO_CANVAS_DRAW_LISTENER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIACAPTUREFROMELEMENT_AUTO_CANVAS_DRAW_LISTENER_H_

#include <memory>
#include "third_party/blink/public/platform/web_canvas_capture_handler.h"
#include "third_party/blink/renderer/core/html/canvas/canvas_draw_listener.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class AutoCanvasDrawListener final
    : public GarbageCollectedFinalized<AutoCanvasDrawListener>,
      public CanvasDrawListener {
  USING_GARBAGE_COLLECTED_MIXIN(AutoCanvasDrawListener);

 public:
  static AutoCanvasDrawListener* Create(
      std::unique_ptr<WebCanvasCaptureHandler>);
  ~AutoCanvasDrawListener() override = default;

  void Trace(blink::Visitor* visitor) override {}

 private:
  AutoCanvasDrawListener(std::unique_ptr<WebCanvasCaptureHandler>);
};

}  // namespace blink

#endif
