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

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_CRYPTO_ALGORITHM_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_CRYPTO_ALGORITHM_H_

#include "third_party/blink/public/platform/web_common.h"
#include "third_party/blink/public/platform/web_private_ptr.h"

#if INSIDE_BLINK
#include <memory>
#endif

namespace blink {

enum WebCryptoOperation {
  kWebCryptoOperationEncrypt,
  kWebCryptoOperationDecrypt,
  kWebCryptoOperationSign,
  kWebCryptoOperationVerify,
  kWebCryptoOperationDigest,
  kWebCryptoOperationGenerateKey,
  kWebCryptoOperationImportKey,
  kWebCryptoOperationGetKeyLength,
  kWebCryptoOperationDeriveBits,
  kWebCryptoOperationWrapKey,
  kWebCryptoOperationUnwrapKey,
  kWebCryptoOperationLast = kWebCryptoOperationUnwrapKey,
};

enum WebCryptoAlgorithmId {
  kWebCryptoAlgorithmIdAesCbc,
  kWebCryptoAlgorithmIdHmac,
  kWebCryptoAlgorithmIdRsaSsaPkcs1v1_5,
  kWebCryptoAlgorithmIdSha1,
  kWebCryptoAlgorithmIdSha256,
  kWebCryptoAlgorithmIdSha384,
  kWebCryptoAlgorithmIdSha512,
  kWebCryptoAlgorithmIdAesGcm,
  kWebCryptoAlgorithmIdRsaOaep,
  kWebCryptoAlgorithmIdAesCtr,
  kWebCryptoAlgorithmIdAesKw,
  kWebCryptoAlgorithmIdRsaPss,
  kWebCryptoAlgorithmIdEcdsa,
  kWebCryptoAlgorithmIdEcdh,
  kWebCryptoAlgorithmIdHkdf,
  kWebCryptoAlgorithmIdPbkdf2,
#if INSIDE_BLINK
  kWebCryptoAlgorithmIdLast = kWebCryptoAlgorithmIdPbkdf2,
#endif
};

enum WebCryptoNamedCurve {
  kWebCryptoNamedCurveP256,
  kWebCryptoNamedCurveP384,
  kWebCryptoNamedCurveP521,
#if INSIDE_BLINK
  kWebCryptoNamedCurveLast = kWebCryptoNamedCurveP521,
#endif
};

enum WebCryptoAlgorithmParamsType {
  kWebCryptoAlgorithmParamsTypeNone,
  kWebCryptoAlgorithmParamsTypeAesCbcParams,
  kWebCryptoAlgorithmParamsTypeAesKeyGenParams,
  kWebCryptoAlgorithmParamsTypeHmacImportParams,
  kWebCryptoAlgorithmParamsTypeHmacKeyGenParams,
  kWebCryptoAlgorithmParamsTypeRsaHashedKeyGenParams,
  kWebCryptoAlgorithmParamsTypeRsaHashedImportParams,
  kWebCryptoAlgorithmParamsTypeAesGcmParams,
  kWebCryptoAlgorithmParamsTypeRsaOaepParams,
  kWebCryptoAlgorithmParamsTypeAesCtrParams,
  kWebCryptoAlgorithmParamsTypeRsaPssParams,
  kWebCryptoAlgorithmParamsTypeEcdsaParams,
  kWebCryptoAlgorithmParamsTypeEcKeyGenParams,
  kWebCryptoAlgorithmParamsTypeEcKeyImportParams,
  kWebCryptoAlgorithmParamsTypeEcdhKeyDeriveParams,
  kWebCryptoAlgorithmParamsTypeAesDerivedKeyParams,
  kWebCryptoAlgorithmParamsTypeHkdfParams,
  kWebCryptoAlgorithmParamsTypePbkdf2Params,
};

struct WebCryptoAlgorithmInfo {
  typedef char ParamsTypeOrUndefined;
  static const ParamsTypeOrUndefined kUndefined = -1;

  // The canonical (case-sensitive) name for the algorithm as a
  // null-terminated C-string literal.
  const char* name;

  // A map from the operation to the expected parameter type of the algorithm.
  // If an operation is not applicable for the algorithm, set to Undefined.
  const ParamsTypeOrUndefined
      operation_to_params_type[kWebCryptoOperationLast + 1];
};

class WebCryptoAesCbcParams;
class WebCryptoAesKeyGenParams;
class WebCryptoHmacImportParams;
class WebCryptoHmacKeyGenParams;
class WebCryptoAesGcmParams;
class WebCryptoRsaOaepParams;
class WebCryptoAesCtrParams;
class WebCryptoRsaHashedKeyGenParams;
class WebCryptoRsaHashedImportParams;
class WebCryptoRsaPssParams;
class WebCryptoEcdsaParams;
class WebCryptoEcKeyGenParams;
class WebCryptoEcKeyImportParams;
class WebCryptoEcdhKeyDeriveParams;
class WebCryptoAesDerivedKeyParams;
class WebCryptoHkdfParams;
class WebCryptoPbkdf2Params;

class WebCryptoAlgorithmParams;
class WebCryptoAlgorithmPrivate;

// The WebCryptoAlgorithm represents a normalized algorithm and its parameters.
//   * Immutable
//   * Threadsafe
//   * Copiable (cheaply)
//
// If WebCryptoAlgorithm "IsNull()" then it is invalid to call any of the other
// methods on it (other than destruction, assignment, or IsNull()).
class WebCryptoAlgorithm {
 public:
#if INSIDE_BLINK
  WebCryptoAlgorithm() = default;
  BLINK_PLATFORM_EXPORT WebCryptoAlgorithm(
      WebCryptoAlgorithmId,
      std::unique_ptr<WebCryptoAlgorithmParams>);
#endif

  BLINK_PLATFORM_EXPORT static WebCryptoAlgorithm CreateNull();
  BLINK_PLATFORM_EXPORT static WebCryptoAlgorithm AdoptParamsAndCreate(
      WebCryptoAlgorithmId,
      WebCryptoAlgorithmParams*);

  // Returns a WebCryptoAlgorithmInfo for the algorithm with the given ID. If
  // the ID is invalid, return 0. The caller can assume the pointer will be
  // valid for the program's entire runtime.
  BLINK_PLATFORM_EXPORT static const WebCryptoAlgorithmInfo*
      LookupAlgorithmInfo(WebCryptoAlgorithmId);

  ~WebCryptoAlgorithm() { Reset(); }

  WebCryptoAlgorithm(const WebCryptoAlgorithm& other) { Assign(other); }
  WebCryptoAlgorithm& operator=(const WebCryptoAlgorithm& other) {
    Assign(other);
    return *this;
  }

  BLINK_PLATFORM_EXPORT bool IsNull() const;

  BLINK_PLATFORM_EXPORT WebCryptoAlgorithmId Id() const;

  BLINK_PLATFORM_EXPORT WebCryptoAlgorithmParamsType ParamsType() const;

  // Retrieves the type-specific parameters. The algorithm contains at most 1
  // type of parameters. Retrieving an invalid parameter will return 0.
  BLINK_PLATFORM_EXPORT const WebCryptoAesCbcParams* AesCbcParams() const;
  BLINK_PLATFORM_EXPORT const WebCryptoAesKeyGenParams* AesKeyGenParams() const;
  BLINK_PLATFORM_EXPORT const WebCryptoHmacImportParams* HmacImportParams()
      const;
  BLINK_PLATFORM_EXPORT const WebCryptoHmacKeyGenParams* HmacKeyGenParams()
      const;
  BLINK_PLATFORM_EXPORT const WebCryptoAesGcmParams* AesGcmParams() const;
  BLINK_PLATFORM_EXPORT const WebCryptoRsaOaepParams* RsaOaepParams() const;
  BLINK_PLATFORM_EXPORT const WebCryptoAesCtrParams* AesCtrParams() const;
  BLINK_PLATFORM_EXPORT const WebCryptoRsaHashedImportParams*
  RsaHashedImportParams() const;
  BLINK_PLATFORM_EXPORT const WebCryptoRsaHashedKeyGenParams*
  RsaHashedKeyGenParams() const;
  BLINK_PLATFORM_EXPORT const WebCryptoRsaPssParams* RsaPssParams() const;
  BLINK_PLATFORM_EXPORT const WebCryptoEcdsaParams* EcdsaParams() const;
  BLINK_PLATFORM_EXPORT const WebCryptoEcKeyGenParams* EcKeyGenParams() const;
  BLINK_PLATFORM_EXPORT const WebCryptoEcKeyImportParams* EcKeyImportParams()
      const;
  BLINK_PLATFORM_EXPORT const WebCryptoEcdhKeyDeriveParams*
  EcdhKeyDeriveParams() const;
  BLINK_PLATFORM_EXPORT const WebCryptoAesDerivedKeyParams*
  AesDerivedKeyParams() const;
  BLINK_PLATFORM_EXPORT const WebCryptoHkdfParams* HkdfParams() const;
  BLINK_PLATFORM_EXPORT const WebCryptoPbkdf2Params* Pbkdf2Params() const;

  // Returns true if the provided algorithm ID is for a hash (in other words,
  // SHA-*)
  BLINK_PLATFORM_EXPORT static bool IsHash(WebCryptoAlgorithmId);
  // Returns true if the provided algorithm ID is for a key derivation function
  BLINK_PLATFORM_EXPORT static bool IsKdf(WebCryptoAlgorithmId);

 private:
  BLINK_PLATFORM_EXPORT void Assign(const WebCryptoAlgorithm& other);
  BLINK_PLATFORM_EXPORT void Reset();

  WebPrivatePtr<WebCryptoAlgorithmPrivate> private_;
};

}  // namespace blink

#endif
