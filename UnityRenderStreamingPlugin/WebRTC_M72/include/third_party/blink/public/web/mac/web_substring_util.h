/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_PUBLIC_WEB_MAC_WEB_SUBSTRING_UTIL_H_
#define THIRD_PARTY_BLINK_PUBLIC_WEB_MAC_WEB_SUBSTRING_UTIL_H_

#include "third_party/blink/public/platform/web_common.h"

#if __OBJC__
@class NSAttributedString;
#else
class NSAttributedString;
#endif

namespace blink {

class WebFrameWidget;
class WebLocalFrame;
struct WebPoint;

class WebSubstringUtil {
 public:
  // Returns an autoreleased NSAttributedString that is the word under
  // the given point inside the given WebFrameWidget or nil on error.
  // Upon return, |baselinePoint| is set to the left baseline point in
  // AppKit coordinates.
  BLINK_EXPORT static NSAttributedString*
  AttributedWordAtPoint(WebFrameWidget*, WebPoint, WebPoint& baseline_point);

  // Returns an autoreleased NSAttributedString that is a substring of the
  // Frame at the given range, or nil on error.
  BLINK_EXPORT static NSAttributedString*
  AttributedSubstringInRange(WebLocalFrame*, size_t location, size_t length);

  // Returns an autoreleased NSAttributedString that is a substring of the
  // Frame at the given range, or nil on error.
  // It also gets the baseline point for the given range for showing
  // dictionary lookup bubble.
  BLINK_EXPORT static NSAttributedString* AttributedSubstringInRange(
      WebLocalFrame*,
      size_t location,
      size_t length,
      WebPoint* baseline_point);
};

}  // namespace blink

#endif
