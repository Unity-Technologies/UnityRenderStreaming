// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_FONT_TEST_HELPERS_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_FONT_TEST_HELPERS_H_

#include "third_party/blink/renderer/platform/fonts/font_description.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

namespace blink {

class Font;

namespace test {

// Reads a font from a specified path, for use in unit tests only.
Font CreateTestFont(const AtomicString& family_name,
                    const String& font_path,
                    float size,
                    const FontDescription::VariantLigatures* = nullptr);

}  // namespace test
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_FONT_TEST_HELPERS_H_
