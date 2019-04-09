// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_ENCODING_TEXT_DECODER_STREAM_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_ENCODING_TEXT_DECODER_STREAM_H_

#include "third_party/blink/renderer/bindings/core/v8/script_value.h"
#include "third_party/blink/renderer/core/streams/transform_stream.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/bindings/trace_wrapper_member.h"
#include "third_party/blink/renderer/platform/wtf/text/text_encoding.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class ExceptionState;
class ReadableStream;
class ScriptState;
class TextDecoderOptions;
class Visitor;
class WritableStream;

// Implements the TextDecoderStream interface as specified at
// https://encoding.spec.whatwg.org/#interface-textdecoderstream.
// Converts a stream of binary data in the form of BufferSource chunks to a
// stream of text data in the form of string chunks. After construction
// functionality is delegated to the owner TransformStream.
class TextDecoderStream final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static TextDecoderStream* Create(ScriptState*,
                                   const String& label,
                                   const TextDecoderOptions*,
                                   ExceptionState&);
  ~TextDecoderStream() override;

  // From text_decoder_stream.idl
  String encoding() const;
  bool fatal() const { return fatal_; }
  bool ignoreBOM() const { return ignore_bom_; }
  ReadableStream* readable() const;
  WritableStream* writable() const;

  void Trace(Visitor* visitor) override;

 private:
  class Transformer;

  TextDecoderStream(ScriptState*,
                    const WTF::TextEncoding&,
                    const TextDecoderOptions*,
                    ExceptionState&);

  const TraceWrapperMember<TransformStream> transform_;
  const WTF::TextEncoding encoding_;
  const bool fatal_;
  const bool ignore_bom_;

  DISALLOW_COPY_AND_ASSIGN(TextDecoderStream);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_ENCODING_TEXT_DECODER_STREAM_H_
