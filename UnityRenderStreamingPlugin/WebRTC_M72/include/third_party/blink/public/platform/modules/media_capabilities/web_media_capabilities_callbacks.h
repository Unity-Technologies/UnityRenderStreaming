// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_MODULES_MEDIA_CAPABILITIES_WEB_MEDIA_CAPABILITIES_CALLBACKS_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_MODULES_MEDIA_CAPABILITIES_WEB_MEDIA_CAPABILITIES_CALLBACKS_H_

#include <memory>

#include "third_party/blink/public/platform/modules/media_capabilities/web_media_capabilities_info.h"
#include "third_party/blink/public/platform/web_callbacks.h"

namespace blink {

using WebMediaCapabilitiesDecodingInfoCallbacks =
    WebCallbacks<std::unique_ptr<WebMediaCapabilitiesInfo>, void>;

using WebMediaCapabilitiesEncodingInfoCallbacks =
    WebCallbacks<std::unique_ptr<WebMediaCapabilitiesInfo>, void>;

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_MODULES_MEDIA_CAPABILITIES_WEB_MEDIA_CAPABILITIES_CALLBACKS_H_
