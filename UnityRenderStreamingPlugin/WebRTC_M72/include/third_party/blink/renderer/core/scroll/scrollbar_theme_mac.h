/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SCROLL_SCROLLBAR_THEME_MAC_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SCROLL_SCROLLBAR_THEME_MAC_H_

#include <AppKit/AppKit.h>

#include "third_party/blink/renderer/core/scroll/ns_scroller_imp_details.h"
#include "third_party/blink/renderer/core/scroll/scrollbar_theme.h"

typedef id ScrollbarPainter;

namespace blink {

class Pattern;

class PLATFORM_EXPORT ScrollbarThemeMac : public ScrollbarTheme {
 public:
  ScrollbarThemeMac();
  ~ScrollbarThemeMac() override;

  void RegisterScrollbar(Scrollbar&) override;
  void UnregisterScrollbar(Scrollbar&) override;

  bool SupportsControlTints() const override { return true; }

  // On Mac, the painting code itself animates the opacity so there's no need
  // to disable in order to make the scrollbars invisible. In fact,
  // disabling/enabling causes invalidations which can cause endless loops as
  // Mac queues up scrollbar paint timers.
  bool ShouldDisableInvisibleScrollbars() const override { return false; }

  TimeDelta InitialAutoscrollTimerDelay() override;
  TimeDelta AutoscrollTimerDelay() override;

  void PaintTickmarks(GraphicsContext&,
                      const Scrollbar&,
                      const IntRect&) override;

  bool ShouldCenterOnThumb(const Scrollbar&, const WebMouseEvent&) override;

  bool ShouldRepaintAllPartsOnInvalidation() const override { return false; }
  ScrollbarPart InvalidateOnThumbPositionChange(
      const Scrollbar&,
      float old_position,
      float new_position) const override;
  void UpdateEnabledState(const Scrollbar&) override;
  int ScrollbarThickness(ScrollbarControlSize = kRegularScrollbar) override;
  bool UsesOverlayScrollbars() const override;
  void UpdateScrollbarOverlayColorTheme(const Scrollbar&) override;
  WebScrollbarButtonsPlacement ButtonsPlacement() const override;

  void SetNewPainterForScrollbar(Scrollbar&, ScrollbarPainter);
  ScrollbarPainter PainterForScrollbar(const Scrollbar&) const;

  void PaintTrackBackground(GraphicsContext&,
                            const Scrollbar&,
                            const IntRect&) override;
  void PaintThumb(GraphicsContext& context,
                  const Scrollbar& scrollbar,
                  const IntRect& rect) override {
    PaintThumbInternal(context, scrollbar, rect, 1.0f);
  }
  void PaintThumbWithOpacity(GraphicsContext& context,
                             const Scrollbar& scrollbar,
                             const IntRect& rect) override {
    PaintThumbInternal(context, scrollbar, rect, ThumbOpacity(scrollbar));
  }

  float ThumbOpacity(const Scrollbar&) const override;

  static NSScrollerStyle RecommendedScrollerStyle();

  // See WebScrollbarTheme for parameters description.
  static void UpdateScrollbarsWithNSDefaults(
      float initial_button_delay,
      float autoscroll_button_delay,
      NSScrollerStyle preferred_scroller_style,
      bool redraw,
      bool jump_on_track_click);

 protected:
  int MaxOverlapBetweenPages() override { return 40; }

  bool ShouldDragDocumentInsteadOfThumb(const Scrollbar&,
                                        const WebMouseEvent&) override;
  int ScrollbarPartToHIPressedState(ScrollbarPart);

  virtual void UpdateButtonPlacement(WebScrollbarButtonsPlacement) {}

  IntRect TrackRect(const Scrollbar&, bool painting = false) override;
  IntRect BackButtonRect(const Scrollbar&,
                         ScrollbarPart,
                         bool painting = false) override;
  IntRect ForwardButtonRect(const Scrollbar&,
                            ScrollbarPart,
                            bool painting = false) override;

  bool HasButtons(const Scrollbar&) override { return false; }
  bool HasThumb(const Scrollbar&) override;

  int MinimumThumbLength(const Scrollbar&) override;

  int TickmarkBorderWidth() override { return 1; }

  void PaintThumbInternal(GraphicsContext&,
                          const Scrollbar&,
                          const IntRect&,
                          float opacity);

  scoped_refptr<Pattern> overhang_pattern_;
};
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SCROLL_SCROLLBAR_THEME_MAC_H_
