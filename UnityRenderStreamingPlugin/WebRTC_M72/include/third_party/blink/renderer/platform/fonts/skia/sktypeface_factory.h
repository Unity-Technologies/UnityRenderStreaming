// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_SKIA_SKTYPEFACE_FACTORY_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_SKIA_SKTYPEFACE_FACTORY_H_

#include "third_party/skia/include/core/SkRefCnt.h"
#include "third_party/skia/include/core/SkTypeface.h"

namespace blink {

// Class to bundle all typeface creation method and hopefully harmonize them in
// the future. Result of reverting the PaintTypeface abstraction, which has the
// benefit of bundling typeface instantiation in one place but needlessly
// introduces a wrapper for SkTypeface.
class SkTypeface_Factory {
 public:
  static sk_sp<SkTypeface> FromFontConfigInterfaceIdAndTtcIndex(int config_id,
                                                                int ttc_index);
  static sk_sp<SkTypeface> FromFilenameAndTtcIndex(const std::string& filename,
                                                   int ttc_index);
  static sk_sp<SkTypeface> FromFamilyNameAndFontStyle(
      const std::string& family_name,
      const SkFontStyle& font_style);
};

}  // namespace blink

#endif
