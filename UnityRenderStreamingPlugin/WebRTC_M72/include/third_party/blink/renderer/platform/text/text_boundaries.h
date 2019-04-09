/*
 * Copyright (C) 2004, 2006 Apple Computer, Inc.  All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_TEXT_TEXT_BOUNDARIES_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_TEXT_TEXT_BOUNDARIES_H_

#include <utility>

#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/text/unicode.h"

namespace blink {

inline bool RequiresContextForWordBoundary(UChar32 ch) {
  return WTF::Unicode::HasLineBreakingPropertyComplexContext(ch);
}

PLATFORM_EXPORT int EndOfFirstWordBoundaryContext(const UChar* characters,
                                                  int length);
PLATFORM_EXPORT int StartOfLastWordBoundaryContext(const UChar* characters,
                                                   int length);

// |UChar*| should be a string in logical order instead of visual order, since
// |FindWordBoundary()| uses ICU, which works on logical order strings
PLATFORM_EXPORT std::pair<int, int> FindWordBackward(const UChar*,
                                                     int len,
                                                     int position);
PLATFORM_EXPORT std::pair<int, int> FindWordForward(const UChar*,
                                                    int len,
                                                    int position);
PLATFORM_EXPORT int FindWordStartBoundary(const UChar*, int len, int position);
PLATFORM_EXPORT int FindWordEndBoundary(const UChar*, int len, int position);
PLATFORM_EXPORT int FindNextWordBackward(const UChar*, int len, int position);
PLATFORM_EXPORT int FindNextWordForward(const UChar*, int len, int position);

}  // namespace blink

#endif
