// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_CONTENT_DECRYPTION_MODULE_ACCESS_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_CONTENT_DECRYPTION_MODULE_ACCESS_H_

#include "third_party/blink/public/platform/web_common.h"

namespace blink {

class WebContentDecryptionModuleResult;
struct WebMediaKeySystemConfiguration;

class BLINK_PLATFORM_EXPORT WebContentDecryptionModuleAccess {
 public:
  virtual ~WebContentDecryptionModuleAccess();
  virtual void CreateContentDecryptionModule(
      WebContentDecryptionModuleResult) = 0;
  virtual WebMediaKeySystemConfiguration GetConfiguration() = 0;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_CONTENT_DECRYPTION_MODULE_ACCESS_H_
