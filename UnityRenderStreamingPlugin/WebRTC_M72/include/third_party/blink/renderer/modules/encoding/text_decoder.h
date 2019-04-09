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

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_ENCODING_TEXT_DECODER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_ENCODING_TEXT_DECODER_H_

#include <memory>
#include "third_party/blink/renderer/bindings/core/v8/array_buffer_or_array_buffer_view.h"
#include "third_party/blink/renderer/modules/encoding/text_decode_options.h"
#include "third_party/blink/renderer/modules/encoding/text_decoder_options.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/text/text_codec.h"
#include "third_party/blink/renderer/platform/wtf/text/text_encoding.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class ExceptionState;

typedef ArrayBufferOrArrayBufferView BufferSource;

class TextDecoder final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static TextDecoder* Create(const String& label,
                             const TextDecoderOptions*,
                             ExceptionState&);
  ~TextDecoder() override;

  // Implement the IDL
  String encoding() const;
  bool fatal() const { return fatal_; }
  bool ignoreBOM() const { return ignore_bom_; }
  String decode(const BufferSource&, const TextDecodeOptions*, ExceptionState&);
  String decode(ExceptionState&);

 private:
  TextDecoder(const WTF::TextEncoding&, bool fatal, bool ignore_bom);

  String decode(const char* start,
                uint32_t length,
                const TextDecodeOptions*,
                ExceptionState&);

  WTF::TextEncoding encoding_;
  std::unique_ptr<WTF::TextCodec> codec_;
  bool fatal_;
  bool ignore_bom_;
  bool bom_seen_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_ENCODING_TEXT_DECODER_H_
