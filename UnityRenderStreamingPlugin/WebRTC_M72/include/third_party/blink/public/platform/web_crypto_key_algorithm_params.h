/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_CRYPTO_KEY_ALGORITHM_PARAMS_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_CRYPTO_KEY_ALGORITHM_PARAMS_H_

#include "third_party/blink/public/platform/web_common.h"
#include "third_party/blink/public/platform/web_crypto_algorithm.h"
#include "third_party/blink/public/platform/web_vector.h"

#if INSIDE_BLINK
#include "third_party/blink/renderer/platform/heap/handle.h"  // nogncheck
#endif

namespace blink {

// Interface used for serializing WebCryptoKeyAlgorithmParams to a javascript
// dictionary.
class WebCryptoKeyAlgorithmDictionary {
#if INSIDE_BLINK
  STACK_ALLOCATED();
#endif
 public:
  virtual ~WebCryptoKeyAlgorithmDictionary() = default;

  virtual void SetString(const char*, const char*) = 0;
  virtual void SetUint(const char*, unsigned) = 0;
  virtual void SetAlgorithm(const char*, const WebCryptoAlgorithm&) = 0;
  virtual void SetUint8Array(const char*, const WebVector<unsigned char>&) = 0;
};

enum WebCryptoKeyAlgorithmParamsType {
  kWebCryptoKeyAlgorithmParamsTypeNone,
  kWebCryptoKeyAlgorithmParamsTypeHmac,
  kWebCryptoKeyAlgorithmParamsTypeAes,
  kWebCryptoKeyAlgorithmParamsTypeRsaHashed,
  kWebCryptoKeyAlgorithmParamsTypeEc,
};

class WebCryptoKeyAlgorithmParams {
 public:
  virtual ~WebCryptoKeyAlgorithmParams() = default;
  virtual WebCryptoKeyAlgorithmParamsType GetType() const {
    return kWebCryptoKeyAlgorithmParamsTypeNone;
  }

  virtual void WriteToDictionary(WebCryptoKeyAlgorithmDictionary*) const = 0;
};

class WebCryptoAesKeyAlgorithmParams : public WebCryptoKeyAlgorithmParams {
 public:
  explicit WebCryptoAesKeyAlgorithmParams(unsigned short length_bits)
      : length_bits_(length_bits) {}

  unsigned short LengthBits() const { return length_bits_; }

  WebCryptoKeyAlgorithmParamsType GetType() const override {
    return kWebCryptoKeyAlgorithmParamsTypeAes;
  }

  void WriteToDictionary(WebCryptoKeyAlgorithmDictionary* dict) const override {
    dict->SetUint("length", length_bits_);
  }

 private:
  unsigned short length_bits_;
};

class WebCryptoHmacKeyAlgorithmParams : public WebCryptoKeyAlgorithmParams {
 public:
  WebCryptoHmacKeyAlgorithmParams(const WebCryptoAlgorithm& hash,
                                  unsigned length_bits)
      : hash_(hash), length_bits_(length_bits) {}

  const WebCryptoAlgorithm& GetHash() const { return hash_; }

  unsigned LengthBits() const { return length_bits_; }

  WebCryptoKeyAlgorithmParamsType GetType() const override {
    return kWebCryptoKeyAlgorithmParamsTypeHmac;
  }

  void WriteToDictionary(WebCryptoKeyAlgorithmDictionary* dict) const override {
    dict->SetAlgorithm("hash", hash_);
    dict->SetUint("length", length_bits_);
  }

 private:
  WebCryptoAlgorithm hash_;
  unsigned length_bits_;
};

class WebCryptoRsaHashedKeyAlgorithmParams
    : public WebCryptoKeyAlgorithmParams {
 public:
  WebCryptoRsaHashedKeyAlgorithmParams(unsigned modulus_length_bits,
                                       const unsigned char* public_exponent,
                                       unsigned public_exponent_size,
                                       const WebCryptoAlgorithm& hash)
      : modulus_length_bits_(modulus_length_bits),
        public_exponent_(public_exponent, public_exponent_size),
        hash_(hash) {}

  unsigned ModulusLengthBits() const { return modulus_length_bits_; }

  const WebVector<unsigned char>& PublicExponent() const {
    return public_exponent_;
  }

  const WebCryptoAlgorithm& GetHash() const { return hash_; }

  WebCryptoKeyAlgorithmParamsType GetType() const override {
    return kWebCryptoKeyAlgorithmParamsTypeRsaHashed;
  }

  void WriteToDictionary(WebCryptoKeyAlgorithmDictionary* dict) const override {
    dict->SetAlgorithm("hash", hash_);
    dict->SetUint("modulusLength", modulus_length_bits_);
    dict->SetUint8Array("publicExponent", public_exponent_);
  }

 private:
  unsigned modulus_length_bits_;
  WebVector<unsigned char> public_exponent_;
  WebCryptoAlgorithm hash_;
};

class WebCryptoEcKeyAlgorithmParams : public WebCryptoKeyAlgorithmParams {
 public:
  explicit WebCryptoEcKeyAlgorithmParams(WebCryptoNamedCurve named_curve)
      : named_curve_(named_curve) {}

  WebCryptoNamedCurve NamedCurve() const { return named_curve_; }

  WebCryptoKeyAlgorithmParamsType GetType() const override {
    return kWebCryptoKeyAlgorithmParamsTypeEc;
  }

  void WriteToDictionary(WebCryptoKeyAlgorithmDictionary* dict) const override {
    switch (named_curve_) {
      case kWebCryptoNamedCurveP256:
        dict->SetString("namedCurve", "P-256");
        break;
      case kWebCryptoNamedCurveP384:
        dict->SetString("namedCurve", "P-384");
        break;
      case kWebCryptoNamedCurveP521:
        dict->SetString("namedCurve", "P-521");
        break;
    }
  }

 private:
  const WebCryptoNamedCurve named_curve_;
};

}  // namespace blink

#endif
