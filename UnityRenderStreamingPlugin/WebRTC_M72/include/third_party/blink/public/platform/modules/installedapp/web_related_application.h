// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_MODULES_INSTALLEDAPP_WEB_RELATED_APPLICATION_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_MODULES_INSTALLEDAPP_WEB_RELATED_APPLICATION_H_

#include "third_party/blink/public/platform/web_string.h"

namespace blink {

struct WebRelatedApplication {
  WebRelatedApplication() = default;

  WebString platform;
  WebString url;
  WebString id;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_MODULES_INSTALLEDAPP_WEB_RELATED_APPLICATION_H_
