// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_PICTURE_MATCHERS_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_PICTURE_MATCHERS_H_

#include "testing/gmock/include/gmock/gmock.h"
#include "third_party/blink/renderer/platform/geometry/float_rect.h"
#include "third_party/blink/renderer/platform/graphics/color.h"

class SkPicture;

namespace blink {

class FloatRect;

// Matches if the picture draws exactly one rectangle, which (after accounting
// for the total transformation matrix and applying any clips inside that
// transform) matches the rect provided, and whose paint has the color
// requested.
// Note that clips which appear outside of a transform are not currently
// supported.
testing::Matcher<const SkPicture&> DrawsRectangle(const FloatRect&, Color);

struct RectWithColor {
  RectWithColor(const FloatRect& rect_arg, const Color& color_arg)
      : rect(rect_arg), color(color_arg) {}
  FloatRect rect;
  Color color;
};

// Same as above, but matches a number of rectangles equal to the size of the
// given vector.
testing::Matcher<const SkPicture&> DrawsRectangles(
    const Vector<RectWithColor>&);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_PICTURE_MATCHERS_H_
