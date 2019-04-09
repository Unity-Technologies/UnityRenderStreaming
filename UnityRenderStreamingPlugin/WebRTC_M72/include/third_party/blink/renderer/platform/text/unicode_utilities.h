/*
 * Copyright (C) 2004, 2006, 2009 Apple Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_TEXT_UNICODE_UTILITIES_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_TEXT_UNICODE_UTILITIES_H_

#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/text/unicode.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

PLATFORM_EXPORT bool IsSeparator(UChar32);
PLATFORM_EXPORT bool IsKanaLetter(UChar character);
PLATFORM_EXPORT bool ContainsKanaLetters(const String&);
PLATFORM_EXPORT void NormalizeCharactersIntoNFCForm(const UChar* characters,
                                                    unsigned length,
                                                    Vector<UChar>& buffer);
PLATFORM_EXPORT void FoldQuoteMarksAndSoftHyphens(UChar* data, size_t length);
PLATFORM_EXPORT void FoldQuoteMarksAndSoftHyphens(String&);
PLATFORM_EXPORT bool CheckOnlyKanaLettersInStrings(const UChar* first_data,
                                                   unsigned first_length,
                                                   const UChar* second_data,
                                                   unsigned second_length);
PLATFORM_EXPORT bool CheckKanaStringsEqual(const UChar* first_data,
                                           unsigned first_length,
                                           const UChar* second_data,
                                           unsigned second_length);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_TEXT_UNICODE_UTILITIES_H_
