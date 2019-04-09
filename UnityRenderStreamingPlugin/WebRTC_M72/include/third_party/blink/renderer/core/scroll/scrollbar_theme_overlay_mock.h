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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SCROLL_SCROLLBAR_THEME_OVERLAY_MOCK_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SCROLL_SCROLLBAR_THEME_OVERLAY_MOCK_H_

#include "third_party/blink/renderer/core/scroll/scrollbar_theme_overlay.h"

namespace blink {

class CORE_EXPORT ScrollbarThemeOverlayMock : public ScrollbarThemeOverlay {
 public:
  ScrollbarThemeOverlayMock()
      : ScrollbarThemeOverlay(3, 4, kDisallowHitTest, Color(128, 128, 128)) {}

  TimeDelta OverlayScrollbarFadeOutDelay() const override { return delay_; }
  TimeDelta OverlayScrollbarFadeOutDuration() const override {
    return TimeDelta();
  }

  void SetOverlayScrollbarFadeOutDelay(TimeDelta delay) { delay_ = delay; }

  void PaintThumb(GraphicsContext& gc,
                  const Scrollbar& scrollbar,
                  const IntRect& rect) override {
    if (!scrollbar.Enabled())
      return;
    ScrollbarThemeOverlay::PaintThumb(gc, scrollbar, rect);
  }

  bool ShouldSnapBackToDragOrigin(const Scrollbar& scrollbar,
                                  const WebMouseEvent& evt) override {
    return false;
  }

  int MinimumThumbLength(const Scrollbar&) override { return 7; }

 private:
  TimeDelta delay_;
  bool IsMockTheme() const final { return true; }
};

}  // namespace blink
#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SCROLL_SCROLLBAR_THEME_OVERLAY_MOCK_H_
