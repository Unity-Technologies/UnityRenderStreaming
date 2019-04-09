// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_ANDROID_FONT_UNIQUE_NAME_LOOKUP_ANDROID_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_ANDROID_FONT_UNIQUE_NAME_LOOKUP_ANDROID_H_

#include "third_party/blink/public/common/font_unique_name_lookup/font_table_matcher.h"
#include "third_party/blink/renderer/platform/fonts/font_unique_name_lookup.h"

#include <memory>

namespace blink {

class FontUniqueNameLookupAndroid : public FontUniqueNameLookup {
 public:
  FontUniqueNameLookupAndroid() = default;
  ~FontUniqueNameLookupAndroid() override;
  sk_sp<SkTypeface> MatchUniqueName(const String& font_unique_name) override;

 private:
  bool EnsureMatchingServiceConnected();
  std::unique_ptr<FontTableMatcher> font_table_matcher_;
  DISALLOW_COPY_AND_ASSIGN(FontUniqueNameLookupAndroid);
};

}  // namespace blink

#endif
