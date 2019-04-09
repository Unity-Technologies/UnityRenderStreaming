// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_TRUSTEDTYPES_TRUSTED_SCRIPT_URL_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_TRUSTEDTYPES_TRUSTED_SCRIPT_URL_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"

namespace WTF {

class String;

}  // namespace WTF

namespace blink {

class CORE_EXPORT TrustedScriptURL final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static TrustedScriptURL* Create(const KURL& url) {
    return new TrustedScriptURL(url);
  }

  // TrustedScriptURL.idl
  String toString() const;

 private:
  TrustedScriptURL(const KURL&);

  KURL url_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_TRUSTEDTYPES_TRUSTED_SCRIPT_URL_H_
