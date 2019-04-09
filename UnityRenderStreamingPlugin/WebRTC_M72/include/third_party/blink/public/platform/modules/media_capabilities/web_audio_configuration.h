// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_MODULES_MEDIA_CAPABILITIES_WEB_AUDIO_CONFIGURATION_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_MODULES_MEDIA_CAPABILITIES_WEB_AUDIO_CONFIGURATION_H_

#include "base/optional.h"
#include "third_party/blink/public/platform/web_string.h"

namespace blink {

// Represents an AudioConfiguration dictionary to be used outside of Blink.
// It is created by Blink and passed to consumers that can assume that all
// required fields are properly set.
struct WebAudioConfiguration {
  WebString mime_type;
  WebString codec;

  // `channels` is Optional too but it will be reflected with `isNull()`.
  WebString channels;
  base::Optional<unsigned> bitrate;
  base::Optional<unsigned> samplerate;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_MODULES_MEDIA_CAPABILITIES_WEB_AUDIO_CONFIGURATION_H_
