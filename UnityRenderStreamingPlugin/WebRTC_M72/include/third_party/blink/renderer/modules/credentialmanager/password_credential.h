// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_CREDENTIALMANAGER_PASSWORD_CREDENTIAL_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_CREDENTIALMANAGER_PASSWORD_CREDENTIAL_H_

#include "third_party/blink/renderer/modules/credentialmanager/credential.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"

namespace blink {

class HTMLFormElement;
class PasswordCredentialData;

class MODULES_EXPORT PasswordCredential final : public Credential {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static PasswordCredential* Create(const PasswordCredentialData*,
                                    ExceptionState&);
  static PasswordCredential* Create(HTMLFormElement*, ExceptionState&);
  static PasswordCredential* Create(const String& id,
                                    const String& password,
                                    const String& name,
                                    const KURL& icon_url);

  // Credential:
  bool IsPasswordCredential() const override;

  // PasswordCredential.idl
  const String& password() const { return password_; }
  const String& name() const { return name_; }
  const KURL& iconURL() const { return icon_url_; }

 private:
  PasswordCredential(const String& id,
                     const String& password,
                     const String& name,
                     const KURL& icon);

  const String password_;
  const String name_;
  const KURL icon_url_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_CREDENTIALMANAGER_PASSWORD_CREDENTIAL_H_
