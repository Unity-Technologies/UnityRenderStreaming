// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CAPABILITIES_MEDIA_CAPABILITIES_DECODING_INFO_CALLBACKS_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CAPABILITIES_MEDIA_CAPABILITIES_DECODING_INFO_CALLBACKS_H_

#include "third_party/blink/public/platform/modules/media_capabilities/web_media_capabilities_callbacks.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise_resolver.h"
#include "third_party/blink/renderer/platform/heap/persistent.h"

namespace blink {

class MediaCapabilitiesDecodingInfoCallbacks
    : public WebMediaCapabilitiesDecodingInfoCallbacks {
 public:
  explicit MediaCapabilitiesDecodingInfoCallbacks(
      ScriptPromiseResolver* resolver);

  ~MediaCapabilitiesDecodingInfoCallbacks() override;

  void OnSuccess(std::unique_ptr<WebMediaCapabilitiesInfo>) override;
  void OnError() override;

 private:
  Persistent<ScriptPromiseResolver> resolver_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CAPABILITIES_MEDIA_CAPABILITIES_QUERY_CALLBACKS_H_
