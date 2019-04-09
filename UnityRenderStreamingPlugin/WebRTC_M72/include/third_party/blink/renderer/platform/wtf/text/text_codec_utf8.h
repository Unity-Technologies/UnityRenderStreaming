/*
 * Copyright (C) 2011 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_WTF_TEXT_TEXT_CODEC_UTF8_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_WTF_TEXT_TEXT_CODEC_UTF8_H_

#include <memory>
#include "third_party/blink/renderer/platform/wtf/text/text_codec.h"

namespace WTF {

class TextCodecUTF8 : public TextCodec {
 public:
  static void RegisterEncodingNames(EncodingNameRegistrar);
  static void RegisterCodecs(TextCodecRegistrar);

 protected:
  TextCodecUTF8() : partial_sequence_size_(0) {}

 private:
  static std::unique_ptr<TextCodec> Create(const TextEncoding&, const void*);

  String Decode(const char*,
                wtf_size_t length,
                FlushBehavior,
                bool stop_on_error,
                bool& saw_error) override;
  CString Encode(const UChar*, wtf_size_t length, UnencodableHandling) override;
  CString Encode(const LChar*, wtf_size_t length, UnencodableHandling) override;

  template <typename CharType>
  CString EncodeCommon(const CharType* characters, wtf_size_t length);

  template <typename CharType>
  bool HandlePartialSequence(CharType*& destination,
                             const uint8_t*& source,
                             const uint8_t* end,
                             bool flush,
                             bool stop_on_error,
                             bool& saw_error);
  void HandleError(int character,
                   UChar*& destination,
                   bool stop_on_error,
                   bool& saw_error);
  void ConsumePartialSequenceBytes(int num_bytes);

  int partial_sequence_size_;
  uint8_t partial_sequence_[U8_MAX_LENGTH];
};

}  // namespace WTF

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_WTF_TEXT_TEXT_CODEC_UTF8_H_
