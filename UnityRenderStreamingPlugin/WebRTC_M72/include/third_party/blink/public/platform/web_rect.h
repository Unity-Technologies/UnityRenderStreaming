/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_RECT_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_RECT_H_

#include "third_party/blink/public/platform/web_common.h"
#include "ui/gfx/geometry/rect.h"

#if INSIDE_BLINK
#include "third_party/blink/renderer/platform/geometry/int_rect.h"  // nogncheck
#else
#include <algorithm>
#include <cmath>
#endif

namespace blink {

struct WebRect {
  int x;
  int y;
  int width;
  int height;

  bool IsEmpty() const { return width <= 0 || height <= 0; }

  WebRect() : x(0), y(0), width(0), height(0) {}

  WebRect(int x, int y, int width, int height)
      : x(x), y(y), width(width), height(height) {}

#if INSIDE_BLINK
  WebRect(const IntRect& r)
      : x(r.X()), y(r.Y()), width(r.Width()), height(r.Height()) {}

  WebRect& operator=(const IntRect& r) {
    x = r.X();
    y = r.Y();
    width = r.Width();
    height = r.Height();
    return *this;
  }

  operator IntRect() const { return IntRect(x, y, width, height); }

  explicit WebRect(const gfx::Rect& r)
      : x(r.x()), y(r.y()), width(r.width()), height(r.height()) {}

  // Note that this conversion clamps the size to be non-negative.
  explicit operator gfx::Rect() const { return gfx::Rect(x, y, width, height); }
#else
  WebRect(const gfx::Rect& r)
      : x(r.x()), y(r.y()), width(r.width()), height(r.height()) {}

  WebRect& operator=(const gfx::Rect& r) {
    x = r.x();
    y = r.y();
    width = r.width();
    height = r.height();
    return *this;
  }

  // Note that this conversion clamps the size to be non-negative.
  operator gfx::Rect() const { return gfx::Rect(x, y, width, height); }
#endif
};

inline bool operator==(const WebRect& a, const WebRect& b) {
  return a.x == b.x && a.y == b.y && a.width == b.width && a.height == b.height;
}

inline bool operator!=(const WebRect& a, const WebRect& b) {
  return !(a == b);
}

}  // namespace blink

#endif
