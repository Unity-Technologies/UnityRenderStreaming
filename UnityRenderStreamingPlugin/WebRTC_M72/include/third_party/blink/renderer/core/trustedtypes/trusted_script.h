// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_TRUSTEDTYPES_TRUSTED_SCRIPT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_TRUSTEDTYPES_TRUSTED_SCRIPT_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class CORE_EXPORT TrustedScript final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static TrustedScript* Create(const String& script) {
    return new TrustedScript(script);
  }

  // TrustedScript.idl
  String toString() const;

 private:
  TrustedScript(const String& script);

  const String script_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_TRUSTEDTYPES_TRUSTED_SCRIPT_H_
