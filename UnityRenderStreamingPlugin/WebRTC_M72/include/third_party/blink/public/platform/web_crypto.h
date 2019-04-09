/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_CRYPTO_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_CRYPTO_H_

#include "base/single_thread_task_runner.h"
#include "third_party/blink/public/platform/web_common.h"
#include "third_party/blink/public/platform/web_crypto_algorithm.h"
#include "third_party/blink/public/platform/web_crypto_key.h"
#include "third_party/blink/public/platform/web_private_ptr.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/platform/web_vector.h"

#include <memory>

#if INSIDE_BLINK
#include "base/memory/scoped_refptr.h"
#include "third_party/blink/renderer/platform/heap/handle.h"  // nogncheck
#endif

namespace blink {

class CryptoResult;
class CryptoResultCancel;
class WebString;

enum WebCryptoErrorType {
  kWebCryptoErrorTypeType,
  kWebCryptoErrorTypeNotSupported,
  kWebCryptoErrorTypeSyntax,
  kWebCryptoErrorTypeInvalidAccess,
  kWebCryptoErrorTypeData,
  kWebCryptoErrorTypeOperation,
};

class WebCryptoResult {
 public:
  WebCryptoResult(const WebCryptoResult& o) { Assign(o); }

  ~WebCryptoResult() { Reset(); }

  WebCryptoResult& operator=(const WebCryptoResult& o) {
    Assign(o);
    return *this;
  }

  // Note that WebString is NOT safe to pass across threads.
  //
  // Error details are surfaced in an exception, and MUST NEVER reveal any
  // secret information such as bytes of the key or plain text. An
  // appropriate error would be something like:
  //   "iv must be 16 bytes long".
  BLINK_PLATFORM_EXPORT void CompleteWithError(WebCryptoErrorType,
                                               const WebString&);

  // Makes a copy of the input data given as a pointer and byte length.
  BLINK_PLATFORM_EXPORT void CompleteWithBuffer(const void*, unsigned);
  BLINK_PLATFORM_EXPORT void CompleteWithJson(const char* utf8_data,
                                              unsigned length);
  BLINK_PLATFORM_EXPORT void CompleteWithBoolean(bool);
  BLINK_PLATFORM_EXPORT void CompleteWithKey(const WebCryptoKey&);
  BLINK_PLATFORM_EXPORT void CompleteWithKeyPair(
      const WebCryptoKey& public_key,
      const WebCryptoKey& private_key);

  // Returns true if the underlying operation was cancelled.
  // This method can be called from any thread.
  BLINK_PLATFORM_EXPORT bool Cancelled() const;

#if INSIDE_BLINK
  BLINK_PLATFORM_EXPORT WebCryptoResult(CryptoResult*,
                                        scoped_refptr<CryptoResultCancel>);
#endif

 private:
  BLINK_PLATFORM_EXPORT void Reset();
  BLINK_PLATFORM_EXPORT void Assign(const WebCryptoResult&);

  WebPrivatePtr<CryptoResult, kWebPrivatePtrDestructionCrossThread> impl_;
  WebPrivatePtr<CryptoResultCancel, kWebPrivatePtrDestructionCrossThread>
      cancel_;
};

class WebCryptoDigestor {
 public:
  virtual ~WebCryptoDigestor() = default;

  // Consume() will return |true| on the successful addition of data to the
  // partially generated digest. It will return |false| when that fails. After
  // a return of |false|, Consume() should not be called again (nor should
  // Finish() be called).
  virtual bool Consume(const unsigned char* data, unsigned data_size) {
    return false;
  }

  // Finish() will return |true| if the digest has been successfully computed
  // and put into the result buffer, otherwise it will return |false|. In
  // either case, neither Finish() nor Consume() should be called again after
  // a call to Finish(). |result_data| is valid until the WebCrytpoDigestor
  // object is destroyed.
  virtual bool Finish(unsigned char*& result_data, unsigned& result_data_size) {
    return false;
  }

 protected:
  WebCryptoDigestor() = default;
};

class WebCrypto {
 public:
  // WebCrypto is the interface for starting one-shot cryptographic
  // operations.
  //
  // -----------------------
  // Completing the request
  // -----------------------
  //
  // Implementations signal completion by calling one of the methods on
  // "result". Only a single result/error should be set for the request.
  // Different operations expect different result types based on the
  // algorithm parameters; see the Web Crypto standard for details.
  //
  // The result can be set either synchronously while handling the request,
  // or asynchronously after the method has returned. When completing
  // asynchronously make a copy of the WebCryptoResult and call it from the
  // |result_task_runner| TaskRunner.
  //
  // If the request was cancelled it is not necessary for implementations to
  // set the result.
  //
  // -----------------------
  // Threading
  // -----------------------
  //
  // The WebCrypto interface will be called from blink threads (main or
  // web worker). All communication back to Blink must be on this same thread
  // (|result_task_runner|).
  //
  // Notably:
  //
  //   * The WebCryptoResult can be copied between threads, however all
  //     methods other than the destructor must be called from the origin
  //     Blink thread (|result_task_runner|).
  //
  //   * WebCryptoKey and WebCryptoAlgorithm ARE threadsafe. They can be
  //     safely copied between threads and accessed. Copying is cheap because
  //     they are internally reference counted.
  //
  // -----------------------
  // Inputs
  // -----------------------
  //
  //   * Data buffers are transfered as WebVectors. Implementations are free
  //     to re-use or transfer their storage.
  //
  //   * All WebCryptoKeys are guaranteeed to be !IsNull().
  //
  //   * All WebCryptoAlgorithms are guaranteed to be !IsNull()
  //
  //   * Look to the Web Crypto spec for an explanation of the parameter. The
  //     method names here have a 1:1 correspondence with those of
  //     crypto.subtle, with the exception of "verify" which is here called
  //     "verifySignature".
  //
  // -----------------------
  // Guarantees on input validity
  // -----------------------
  //
  // Implementations MUST carefully sanitize algorithm inputs before using
  // them, as they come directly from the user. Few checks have been done on
  // algorithm parameters prior to passing to the embedder.
  //
  // Only the following checks can be assumed as having already passed:
  //
  //  * The key is extractable when calling into ExportKey/WrapKey.
  //  * The key usages permit the operation being requested.
  //  * The key's algorithm matches that of the requested operation.
  //
  virtual void Encrypt(
      const WebCryptoAlgorithm&,
      const WebCryptoKey&,
      WebVector<unsigned char> data,
      WebCryptoResult result,
      scoped_refptr<base::SingleThreadTaskRunner> task_runner) {
    result.CompleteWithError(kWebCryptoErrorTypeNotSupported, "");
  }
  virtual void Decrypt(
      const WebCryptoAlgorithm&,
      const WebCryptoKey&,
      WebVector<unsigned char> data,
      WebCryptoResult result,
      scoped_refptr<base::SingleThreadTaskRunner> task_runner) {
    result.CompleteWithError(kWebCryptoErrorTypeNotSupported, "");
  }
  virtual void Sign(const WebCryptoAlgorithm&,
                    const WebCryptoKey&,
                    WebVector<unsigned char> data,
                    WebCryptoResult result,
                    scoped_refptr<base::SingleThreadTaskRunner> task_runner) {
    result.CompleteWithError(kWebCryptoErrorTypeNotSupported, "");
  }
  virtual void VerifySignature(
      const WebCryptoAlgorithm&,
      const WebCryptoKey&,
      WebVector<unsigned char> signature,
      WebVector<unsigned char> data,
      WebCryptoResult result,
      scoped_refptr<base::SingleThreadTaskRunner> task_runner) {
    result.CompleteWithError(kWebCryptoErrorTypeNotSupported, "");
  }
  virtual void Digest(const WebCryptoAlgorithm&,
                      WebVector<unsigned char> data,
                      WebCryptoResult result,
                      scoped_refptr<base::SingleThreadTaskRunner> task_runner) {
    result.CompleteWithError(kWebCryptoErrorTypeNotSupported, "");
  }
  virtual void GenerateKey(
      const WebCryptoAlgorithm&,
      bool extractable,
      WebCryptoKeyUsageMask,
      WebCryptoResult result,
      scoped_refptr<base::SingleThreadTaskRunner> task_runner) {
    result.CompleteWithError(kWebCryptoErrorTypeNotSupported, "");
  }
  virtual void ImportKey(
      WebCryptoKeyFormat,
      WebVector<unsigned char> key_data,
      const WebCryptoAlgorithm&,
      bool extractable,
      WebCryptoKeyUsageMask,
      WebCryptoResult result,
      scoped_refptr<base::SingleThreadTaskRunner> task_runner) {
    result.CompleteWithError(kWebCryptoErrorTypeNotSupported, "");
  }
  virtual void ExportKey(
      WebCryptoKeyFormat,
      const WebCryptoKey&,
      WebCryptoResult result,
      scoped_refptr<base::SingleThreadTaskRunner> task_runner) {
    result.CompleteWithError(kWebCryptoErrorTypeNotSupported, "");
  }
  virtual void WrapKey(
      WebCryptoKeyFormat,
      const WebCryptoKey& key,
      const WebCryptoKey& wrapping_key,
      const WebCryptoAlgorithm&,
      WebCryptoResult result,
      scoped_refptr<base::SingleThreadTaskRunner> task_runner) {
    result.CompleteWithError(kWebCryptoErrorTypeNotSupported, "");
  }
  virtual void UnwrapKey(
      WebCryptoKeyFormat,
      WebVector<unsigned char> wrapped_key,
      const WebCryptoKey&,
      const WebCryptoAlgorithm& unwrap_algorithm,
      const WebCryptoAlgorithm& unwrapped_key_algorithm,
      bool extractable,
      WebCryptoKeyUsageMask,
      WebCryptoResult result,
      scoped_refptr<base::SingleThreadTaskRunner> task_runner) {
    result.CompleteWithError(kWebCryptoErrorTypeNotSupported, "");
  }
  virtual void DeriveBits(
      const WebCryptoAlgorithm&,
      const WebCryptoKey&,
      unsigned length,
      WebCryptoResult result,
      scoped_refptr<base::SingleThreadTaskRunner> task_runner) {
    result.CompleteWithError(kWebCryptoErrorTypeNotSupported, "");
  }
  virtual void DeriveKey(
      const WebCryptoAlgorithm& algorithm,
      const WebCryptoKey& base_key,
      const WebCryptoAlgorithm& import_algorithm,
      const WebCryptoAlgorithm& key_length_algorithm,
      bool extractable,
      WebCryptoKeyUsageMask,
      WebCryptoResult result,
      scoped_refptr<base::SingleThreadTaskRunner> task_runner) {
    result.CompleteWithError(kWebCryptoErrorTypeNotSupported, "");
  }

  // This is the exception to the "Completing the request" guarantees
  // outlined above. This is useful for Blink internal crypto and is not part
  // of the WebCrypto standard. CreateDigestor must provide the result via
  // the WebCryptoDigestor object synchronously. This will never return null.
  virtual std::unique_ptr<WebCryptoDigestor> CreateDigestor(
      WebCryptoAlgorithmId algorithm_id) {
    return nullptr;
  }

  // -----------------------
  // Structured clone
  // -----------------------
  //
  // DeserializeKeyForClone() and SerializeKeyForClone() are used for
  // implementing structured cloning of WebCryptoKey.
  //
  // Blink is responsible for saving and restoring all of the attributes of
  // WebCryptoKey EXCEPT for the actual key data:
  //
  // In other words, Blink takes care of serializing:
  //   * Key usages
  //   * Key extractability
  //   * Key algorithm
  //   * Key type (public, private, secret)
  //
  // The embedder is responsible for saving the key data itself.
  //
  // Visibility of the serialized key data:
  //
  // The serialized key data will NOT be visible to web pages. So if the
  // serialized format were to include key bytes as plain text, this wouldn't
  // make it available to web pages.
  //
  // Longevity of the key data:
  //
  // The serialized key data is intended to be long lived (years) and MUST
  // be using a stable format. For instance a key might be persisted to
  // IndexedDB and should be able to be deserialized correctly in the
  // future.
  //
  // Error handling and asynchronous completion:
  //
  // Serialization/deserialization must complete synchronously, and will
  // block the JavaScript thread.
  //
  // The only reasons to fail serialization/deserialization are:
  //   * Key serialization not yet implemented
  //   * The bytes to deserialize were corrupted

  // Creates a new key given key data which was written using
  // SerializeKeyForClone(). Returns true on success.
  virtual bool DeserializeKeyForClone(const WebCryptoKeyAlgorithm&,
                                      WebCryptoKeyType,
                                      bool extractable,
                                      WebCryptoKeyUsageMask,
                                      const unsigned char* key_data,
                                      unsigned key_data_size,
                                      WebCryptoKey&) {
    return false;
  }

  // Writes the key data into the given WebVector.
  // Returns true on success.
  virtual bool SerializeKeyForClone(const WebCryptoKey&,
                                    WebVector<unsigned char>&) {
    return false;
  }

 protected:
  virtual ~WebCrypto() = default;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_CRYPTO_H_
