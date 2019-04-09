// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_ENCRYPTEDMEDIA_MEDIA_KEY_SYSTEM_ACCESS_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_ENCRYPTEDMEDIA_MEDIA_KEY_SYSTEM_ACCESS_H_

#include <memory>
#include "third_party/blink/public/platform/web_content_decryption_module_access.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise.h"
#include "third_party/blink/renderer/modules/encryptedmedia/media_key_system_configuration.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

namespace blink {

class MediaKeySystemAccess final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  MediaKeySystemAccess(const String& key_system,
                       std::unique_ptr<WebContentDecryptionModuleAccess>);
  ~MediaKeySystemAccess() override;

  const String& keySystem() const { return key_system_; }
  MediaKeySystemConfiguration* getConfiguration() const;
  ScriptPromise createMediaKeys(ScriptState*);

 private:
  const String key_system_;
  std::unique_ptr<WebContentDecryptionModuleAccess> access_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_ENCRYPTEDMEDIA_MEDIA_KEY_SYSTEM_ACCESS_H_
