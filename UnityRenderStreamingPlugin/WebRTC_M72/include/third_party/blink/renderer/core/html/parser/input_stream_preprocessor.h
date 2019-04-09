/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 * Copyright (C) 2009 Torch Mobile, Inc. http://www.torchmobile.com/
 * Copyright (C) 2013 Google, Inc. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_PARSER_INPUT_STREAM_PREPROCESSOR_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_PARSER_INPUT_STREAM_PREPROCESSOR_H_

#include "base/macros.h"
#include "third_party/blink/renderer/platform/text/segmented_string.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace blink {

const LChar kEndOfFileMarker = 0;

// http://www.whatwg.org/specs/web-apps/current-work/#preprocessing-the-input-stream
template <typename Tokenizer>
class InputStreamPreprocessor {
  DISALLOW_NEW();

 public:
  InputStreamPreprocessor(Tokenizer* tokenizer) : tokenizer_(tokenizer) {
    Reset();
  }

  ALWAYS_INLINE UChar NextInputCharacter() const {
    return next_input_character_;
  }

  // Returns whether we succeeded in peeking at the next character.
  // The only way we can fail to peek is if there are no more
  // characters in |source| (after collapsing \r\n, etc).
  ALWAYS_INLINE bool Peek(SegmentedString& source) {
    next_input_character_ = source.CurrentChar();

    // Every branch in this function is expensive, so we have a
    // fast-reject branch for characters that don't require special
    // handling. Please run the parser benchmark whenever you touch
    // this function. It's very hot.
    static const UChar kSpecialCharacterMask = '\n' | '\r' | '\0';
    if (next_input_character_ & ~kSpecialCharacterMask) {
      skip_next_new_line_ = false;
      return true;
    }
    return ProcessNextInputCharacter(source);
  }

  // Returns whether there are more characters in |source| after advancing.
  ALWAYS_INLINE bool Advance(SegmentedString& source) {
    source.AdvanceAndUpdateLineNumber();
    if (source.IsEmpty())
      return false;
    return Peek(source);
  }

  bool SkipNextNewLine() const { return skip_next_new_line_; }

  void Reset(bool skip_next_new_line = false) {
    next_input_character_ = '\0';
    skip_next_new_line_ = skip_next_new_line;
  }

 private:
  bool ProcessNextInputCharacter(SegmentedString& source) {
  ProcessAgain:
    DCHECK_EQ(next_input_character_, source.CurrentChar());

    if (next_input_character_ == '\n' && skip_next_new_line_) {
      skip_next_new_line_ = false;
      source.AdvancePastNewlineAndUpdateLineNumber();
      if (source.IsEmpty())
        return false;
      next_input_character_ = source.CurrentChar();
    }
    if (next_input_character_ == '\r') {
      next_input_character_ = '\n';
      skip_next_new_line_ = true;
    } else {
      skip_next_new_line_ = false;
      // FIXME: The spec indicates that the surrogate pair range as well as a
      // number of specific character values are parse errors and should be
      // replaced by the replacement character. We suspect this is a problem
      // with the spec as doing that filtering breaks surrogate pair handling
      // and causes us not to match Minefield.
      if (next_input_character_ == '\0' &&
          !ShouldTreatNullAsEndOfFileMarker(source)) {
        if (tokenizer_->ShouldSkipNullCharacters()) {
          source.AdvancePastNonNewline();
          if (source.IsEmpty())
            return false;
          next_input_character_ = source.CurrentChar();
          goto ProcessAgain;
        }
        next_input_character_ = 0xFFFD;
      }
    }
    return true;
  }

  bool ShouldTreatNullAsEndOfFileMarker(SegmentedString& source) const {
    return source.IsClosed() && source.length() == 1;
  }

  Tokenizer* tokenizer_;

  // http://www.whatwg.org/specs/web-apps/current-work/#next-input-character
  UChar next_input_character_;
  bool skip_next_new_line_;

  DISALLOW_COPY_AND_ASSIGN(InputStreamPreprocessor);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_PARSER_INPUT_STREAM_PREPROCESSOR_H_
