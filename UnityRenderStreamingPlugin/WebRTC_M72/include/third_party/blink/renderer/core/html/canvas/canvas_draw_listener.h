// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_CANVAS_CANVAS_DRAW_LISTENER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_CANVAS_CANVAS_DRAW_LISTENER_H_

#include <memory>

#include "base/memory/weak_ptr.h"
#include "third_party/blink/public/platform/web_canvas_capture_handler.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/skia/include/core/SkRefCnt.h"

class SkImage;

namespace blink {

class WebGraphicsContext3DProviderWrapper;

class CORE_EXPORT CanvasDrawListener : public GarbageCollectedMixin {
 public:
  virtual ~CanvasDrawListener();
  virtual void SendNewFrame(sk_sp<SkImage>,
                            base::WeakPtr<WebGraphicsContext3DProviderWrapper>);
  bool NeedsNewFrame() const;
  void RequestFrame();

 protected:
  explicit CanvasDrawListener(std::unique_ptr<WebCanvasCaptureHandler>);

  bool frame_capture_requested_;
  std::unique_ptr<WebCanvasCaptureHandler> handler_;
};

}  // namespace blink

#endif
