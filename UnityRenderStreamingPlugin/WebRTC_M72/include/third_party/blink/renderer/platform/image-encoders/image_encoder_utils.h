// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_IMAGE_ENCODERS_IMAGE_ENCODER_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_IMAGE_ENCODERS_IMAGE_ENCODER_UTILS_H_

#include "third_party/blink/renderer/platform/graphics/graphics_types.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class PLATFORM_EXPORT ImageEncoderUtils {
 public:
  enum EncodeReason {
    kEncodeReasonToDataURL = 0,
    kEncodeReasonToBlobCallback = 1,
    kEncodeReasonConvertToBlobPromise = 2,
    kNumberOfEncodeReasons
  };

  // Default image mime type for toDataURL and toBlob functions
  static const char kDefaultRequestedMimeType[];
  static const ImageEncodingMimeType kDefaultEncodingMimeType;

  static ImageEncodingMimeType ToEncodingMimeType(const String&,
                                                  const EncodeReason);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_IMAGE_ENCODERS_IMAGE_ENCODER_UTILS_H_
