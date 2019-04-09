// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_MEDIA_KEY_SYSTEM_CONFIGURATION_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_MEDIA_KEY_SYSTEM_CONFIGURATION_H_

#include "third_party/blink/public/platform/web_encrypted_media_types.h"
#include "third_party/blink/public/platform/web_media_key_system_media_capability.h"
#include "third_party/blink/public/platform/web_vector.h"

namespace blink {

struct WebMediaKeySystemConfiguration {
  enum class Requirement {
    kRequired,
    kOptional,
    kNotAllowed,
  };

  WebString label;
  WebVector<WebEncryptedMediaInitDataType> init_data_types;
  WebVector<WebMediaKeySystemMediaCapability> audio_capabilities;
  WebVector<WebMediaKeySystemMediaCapability> video_capabilities;
  Requirement distinctive_identifier = Requirement::kOptional;
  Requirement persistent_state = Requirement::kOptional;
  WebVector<WebEncryptedMediaSessionType> session_types;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_MEDIA_KEY_SYSTEM_CONFIGURATION_H_
