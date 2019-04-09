/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCROLL_SCROLL_TYPES_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCROLL_SCROLL_TYPES_H_

#include "third_party/blink/public/platform/web_gesture_event.h"
#include "third_party/blink/public/platform/web_scroll_types.h"
#include "third_party/blink/renderer/platform/geometry/float_point.h"
#include "third_party/blink/renderer/platform/wtf/assertions.h"

namespace blink {

// A ScrollOffset represents an offset from the scroll origin of a
// ScrollableArea.  Note that "scroll origin" is not the same as the layout
// concept of "location", nor is it necessarily coincident with the top/left of
// the ScrollableArea's overflow rect.  See core/layout/README.md for more
// information.
typedef FloatSize ScrollOffset;

inline ScrollOffset ToScrollOffset(const FloatPoint& p) {
  return ScrollOffset(p.X(), p.Y());
}

// Platform overlay scrollbars are controlled and painted by the operating
// system (e.g., OSX and Android).  CSS overlay scrollbars are created by
// setting overflow:overlay, and they are painted by chromium.
enum OverlayScrollbarClipBehavior {
  kIgnorePlatformOverlayScrollbarSize,
  kIgnorePlatformAndCSSOverlayScrollbarSize,
  kExcludeOverlayScrollbarSizeForHitTesting
};

using ScrollDirection = WebScrollDirection;
using ScrollGranularity = WebScrollGranularity;

enum ScrollDirectionPhysical {
  kScrollUp,
  kScrollDown,
  kScrollLeft,
  kScrollRight
};

enum ScrollType {
  kUserScroll,
  kProgrammaticScroll,
  kClampingScroll,
  kCompositorScroll,
  kAnchoringScroll,
  // These are programmatic sequenced scrolls from SmoothScrollSequencer.
  // SetScrollOffset called with kSequencedScroll should not abort the smooth
  // scroll sequence.
  kSequencedScroll
};

// An explicit scroll is one that was requested by the user or the webpage.
// An implicit scroll is a side effect of a layout change.
inline bool IsExplicitScrollType(ScrollType scroll_type) {
  return scroll_type == kUserScroll || scroll_type == kProgrammaticScroll ||
         scroll_type == kCompositorScroll || scroll_type == kSequencedScroll;
}

// Convert logical scroll direction to physical. Physical scroll directions are
// unaffected.
inline ScrollDirectionPhysical ToPhysicalDirection(ScrollDirection direction,
                                                   bool is_vertical,
                                                   bool is_flipped) {
  switch (direction) {
    case kScrollBlockDirectionBackward: {
      if (is_vertical) {
        if (!is_flipped)
          return kScrollUp;
        return kScrollDown;
      }
      if (!is_flipped)
        return kScrollLeft;
      return kScrollRight;
    }
    case kScrollBlockDirectionForward: {
      if (is_vertical) {
        if (!is_flipped)
          return kScrollDown;
        return kScrollUp;
      }
      if (!is_flipped)
        return kScrollRight;
      return kScrollLeft;
    }
    case kScrollInlineDirectionBackward: {
      if (is_vertical) {
        if (!is_flipped)
          return kScrollLeft;
        return kScrollRight;
      }
      if (!is_flipped)
        return kScrollUp;
      return kScrollDown;
    }
    case kScrollInlineDirectionForward: {
      if (is_vertical) {
        if (!is_flipped)
          return kScrollRight;
        return kScrollLeft;
      }
      if (!is_flipped)
        return kScrollDown;
      return kScrollUp;
    }
    // Direction is already physical
    case kScrollUpIgnoringWritingMode:
      return kScrollUp;
    case kScrollDownIgnoringWritingMode:
      return kScrollDown;
    case kScrollLeftIgnoringWritingMode:
      return kScrollLeft;
    case kScrollRightIgnoringWritingMode:
      return kScrollRight;
    default:
      NOTREACHED();
      break;
  }
  return kScrollUp;
}

inline ScrollDirection ToScrollDirection(ScrollDirectionPhysical direction) {
  switch (direction) {
    case kScrollUp:
      return kScrollUpIgnoringWritingMode;
    case kScrollDown:
      return kScrollDownIgnoringWritingMode;
    case kScrollLeft:
      return kScrollLeftIgnoringWritingMode;
    case kScrollRight:
      return kScrollRightIgnoringWritingMode;
    default:
      NOTREACHED();
      break;
  }
  return kScrollUpIgnoringWritingMode;
}

enum ScrollInertialPhase {
  kScrollInertialPhaseUnknown,
  kScrollInertialPhaseNonMomentum,
  kScrollInertialPhaseMomentum
};

enum ScrollbarOrientation { kHorizontalScrollbar, kVerticalScrollbar };

enum ScrollOrientation { kHorizontalScroll, kVerticalScroll };

enum ScrollbarMode { kScrollbarAuto, kScrollbarAlwaysOff, kScrollbarAlwaysOn };

enum ScrollbarControlSize { kRegularScrollbar, kSmallScrollbar };

typedef unsigned ScrollbarControlState;

enum ScrollbarControlStateMask {
  kActiveScrollbarState = 1,
  kEnabledScrollbarState = 1 << 1,
  kPressedScrollbarState = 1 << 2
};

enum ScrollbarPart {
  kNoPart = 0,
  kBackButtonStartPart = 1,
  kForwardButtonStartPart = 1 << 1,
  kBackTrackPart = 1 << 2,
  kThumbPart = 1 << 3,
  kForwardTrackPart = 1 << 4,
  kBackButtonEndPart = 1 << 5,
  kForwardButtonEndPart = 1 << 6,
  kScrollbarBGPart = 1 << 7,
  kTrackBGPart = 1 << 8,
  kAllParts = 0xffffffff
};

enum ScrollbarOverlayColorTheme {
  kScrollbarOverlayColorThemeDark,
  kScrollbarOverlayColorThemeLight
};

enum ScrollBehavior {
  kScrollBehaviorAuto,
  kScrollBehaviorInstant,
  kScrollBehaviorSmooth,
};

// The result of an attempt to scroll. If didScroll is true, then
// unusedScrollDelta gives the amount of the scroll delta that was not consumed
// by scrolling.
struct ScrollResult {
  STACK_ALLOCATED();

 public:
  explicit ScrollResult()
      : did_scroll_x(false),
        did_scroll_y(false),
        unused_scroll_delta_x(0),
        unused_scroll_delta_y(0) {}
  ScrollResult(bool did_scroll_x,
               bool did_scroll_y,
               float unused_scroll_delta_x,
               float unused_scroll_delta_y)
      : did_scroll_x(did_scroll_x),
        did_scroll_y(did_scroll_y),
        unused_scroll_delta_x(unused_scroll_delta_x),
        unused_scroll_delta_y(unused_scroll_delta_y) {}

  bool DidScroll() { return did_scroll_x || did_scroll_y; }

  bool did_scroll_x;
  bool did_scroll_y;

  // In pixels.
  float unused_scroll_delta_x;
  float unused_scroll_delta_y;
};

inline ScrollOffset ToScrollDelta(ScrollbarOrientation orientation,
                                  float delta) {
  return orientation == kHorizontalScrollbar ? ScrollOffset(delta, 0.0f)
                                             : ScrollOffset(0.0f, delta);
}

inline ScrollOffset ToScrollDelta(ScrollDirectionPhysical dir, float delta) {
  if (dir == kScrollUp || dir == kScrollLeft)
    delta = -delta;

  return (dir == kScrollLeft || dir == kScrollRight) ? ScrollOffset(delta, 0)
                                                     : ScrollOffset(0, delta);
}

inline ScrollGranularity ToPlatformScrollGranularity(
    WebGestureEvent::ScrollUnits units) {
  switch (units) {
    case WebGestureEvent::ScrollUnits::kPrecisePixels:
      return ScrollGranularity::kScrollByPrecisePixel;
    case WebGestureEvent::ScrollUnits::kPixels:
      return ScrollGranularity::kScrollByPixel;
    case WebGestureEvent::ScrollUnits::kPage:
      return ScrollGranularity::kScrollByPage;
    default:
      NOTREACHED();
      return ScrollGranularity::kScrollByPrecisePixel;
  }
}

typedef unsigned ScrollbarControlPartMask;

}  // namespace blink

#endif
