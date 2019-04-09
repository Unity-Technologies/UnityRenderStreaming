// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_ENCRYPTEDMEDIA_MEDIA_KEYS_GET_STATUS_FOR_POLICY_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_ENCRYPTEDMEDIA_MEDIA_KEYS_GET_STATUS_FOR_POLICY_H_

#include "third_party/blink/renderer/bindings/core/v8/script_promise.h"

namespace blink {

class MediaKeys;
class MediaKeysPolicy;
class ScriptState;

class MediaKeysGetStatusForPolicy {
 public:
  static ScriptPromise getStatusForPolicy(ScriptState*,
                                          MediaKeys&,
                                          const MediaKeysPolicy*);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_ENCRYPTEDMEDIA_MEDIA_KEYS_GET_STATUS_FOR_POLICY_H_
