// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_ENCRYPTEDMEDIA_CONTENT_DECRYPTION_MODULE_RESULT_PROMISE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_ENCRYPTEDMEDIA_CONTENT_DECRYPTION_MODULE_RESULT_PROMISE_H_

#include "third_party/blink/public/platform/web_encrypted_media_key_information.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise_resolver.h"
#include "third_party/blink/renderer/platform/bindings/exception_code.h"
#include "third_party/blink/renderer/platform/content_decryption_module_result.h"

namespace blink {

ExceptionCode WebCdmExceptionToExceptionCode(
    WebContentDecryptionModuleException);

// This class wraps the promise resolver to simplify creation of
// ContentDecryptionModuleResult objects. The default implementations of the
// complete(), completeWithSession(), etc. methods will reject the promise
// with an error. It needs to be subclassed and the appropriate complete()
// method overridden to resolve the promise as needed.
//
// Subclasses need to keep a Member<> to the object that created them so
// that the creator remains around as long as this promise is pending. This
// promise is not referenced by the object that created it (e.g MediaKeys,
// MediaKeySession, Navigator.requestMediaKeySystemAccess), so this promise
// may be cleaned up before or after it's creator once both become unreachable.
// If it is after, the destruction of the creator may trigger this promise,
// so use isValidToFulfillPromise() to verify that it is safe to fulfill
// the promise.
class ContentDecryptionModuleResultPromise
    : public ContentDecryptionModuleResult {
 public:
  ~ContentDecryptionModuleResultPromise() override;

  // ContentDecryptionModuleResult implementation.
  void Complete() override;
  void CompleteWithContentDecryptionModule(
      WebContentDecryptionModule*) override;
  void CompleteWithSession(
      WebContentDecryptionModuleResult::SessionStatus) override;
  void CompleteWithKeyStatus(
      WebEncryptedMediaKeyInformation::KeyStatus) override;
  void CompleteWithError(WebContentDecryptionModuleException,
                         unsigned long system_code,
                         const WebString&) override;

  // It is only valid to call this before completion.
  ScriptPromise Promise();

  void Trace(blink::Visitor*) override;

 protected:
  // |interface_name| and |property_name| must have static life time.
  explicit ContentDecryptionModuleResultPromise(ScriptState*,
                                                const char* interface_name,
                                                const char* property_name);

  // Resolves the promise with |value|. Used by subclasses to resolve the
  // promise.
  template <typename... T>
  void Resolve(T... value) {
    DCHECK(IsValidToFulfillPromise());

    resolver_->Resolve(value...);
    resolver_.Clear();
  }

  // Rejects the promise with a DOMException.
  void Reject(ExceptionCode, const String& error_message);

  ExecutionContext* GetExecutionContext() const;

  // Determine if it's OK to resolve/reject this promise.
  bool IsValidToFulfillPromise();

 private:
  Member<ScriptPromiseResolver> resolver_;
  const char* const interface_name_;
  const char* const property_name_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_ENCRYPTEDMEDIA_CONTENT_DECRYPTION_MODULE_RESULT_PROMISE_H_
