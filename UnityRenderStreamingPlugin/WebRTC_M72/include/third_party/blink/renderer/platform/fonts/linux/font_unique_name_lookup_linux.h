// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_LINUX_FONT_UNIQUE_NAME_LOOKUP_LINUX_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_LINUX_FONT_UNIQUE_NAME_LOOKUP_LINUX_H_

#include "third_party/blink/renderer/platform/fonts/font_unique_name_lookup.h"

#include <memory>

namespace blink {

class FontUniqueNameLookupLinux : public FontUniqueNameLookup {
 public:
  FontUniqueNameLookupLinux() = default;
  ~FontUniqueNameLookupLinux() override;
  sk_sp<SkTypeface> MatchUniqueName(const String& font_unique_name) override;

 private:
  DISALLOW_COPY_AND_ASSIGN(FontUniqueNameLookupLinux);
};

}  // namespace blink

#endif
