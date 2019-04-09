/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_FULLSCREEN_CONTROLLER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_FULLSCREEN_CONTROLLER_H_

#include <memory>

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/platform/geometry/float_point.h"
#include "third_party/blink/renderer/platform/geometry/int_size.h"
#include "third_party/blink/renderer/platform/graphics/color.h"
#include "third_party/blink/renderer/platform/heap/persistent.h"

namespace blink {

class Element;
class FullscreenOptions;
class WebViewImpl;

// FullscreenController is a per-WebView class that manages the transition into
// and out of fullscreen, including restoring scroll offset and scale after
// exiting fullscreen. It is (indirectly) used by the Fullscreen class.
class CORE_EXPORT FullscreenController {
 public:
  static std::unique_ptr<FullscreenController> Create(WebViewImpl*);

  // Called by Fullscreen (via ChromeClient) to request entering or exiting
  // fullscreen.
  void EnterFullscreen(LocalFrame&, const FullscreenOptions*);
  void ExitFullscreen(LocalFrame&);

  // Called by content::RenderWidget (via WebWidget) to notify that we've
  // entered or exited fullscreen. This can be because we requested it, or it
  // can be initiated by the browser directly.
  void DidEnterFullscreen();
  void DidExitFullscreen();

  // Called by Fullscreen (via ChromeClient) to notify that the fullscreen
  // element has changed.
  void FullscreenElementChanged(Element* old_element, Element* new_element);

  bool IsFullscreenOrTransitioning() const { return state_ != State::kInitial; }

  void UpdateSize();

 protected:
  explicit FullscreenController(WebViewImpl*);

 private:
  void UpdatePageScaleConstraints(bool reset_constraints);
  void RestoreBackgroundColorOverride();

  WebViewImpl* web_view_base_;

  // State is used to avoid unnecessary enter/exit requests.
  enum class State {
    kInitial,
    kEnteringFullscreen,
    kFullscreen,
    kExitingFullscreen,
  };
  State state_ = State::kInitial;

  bool initial_background_color_override_enabled_ = false;
  RGBA32 initial_background_color_override_ = Color::kTransparent;

  using PendingFullscreenSet = HeapLinkedHashSet<WeakMember<LocalFrame>>;
  Persistent<PendingFullscreenSet> pending_frames_;
};

}  // namespace blink

#endif
