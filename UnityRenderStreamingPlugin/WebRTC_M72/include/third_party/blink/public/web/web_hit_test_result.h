/*
* Copyright (C) 2012 Google Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* 1.  Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_HIT_TEST_RESULT_H_
#define THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_HIT_TEST_RESULT_H_

#include "third_party/blink/public/platform/web_private_ptr.h"

namespace blink {

class HitTestResult;
class WebElement;
class WebHitTestResultPrivate;
class WebNode;
class WebURL;
struct WebPoint;

// Properties of a hit test result, i.e. properties of the nodes at a given
// point (the hit point) on the page. Both urls may be populated at the same
// time, for example in the instance of an <img> inside an <a>.
class WebHitTestResult {
 public:
  WebHitTestResult() = default;
  WebHitTestResult(const WebHitTestResult& info) { Assign(info); }
  ~WebHitTestResult() { Reset(); }

  BLINK_EXPORT void Assign(const WebHitTestResult&);
  BLINK_EXPORT void Reset();
  BLINK_EXPORT bool IsNull() const;

  // The node that was hit (only one for point-based tests).
  BLINK_EXPORT WebNode GetNode() const;

  // Coordinates of the point that was hit. Relative to the node.
  BLINK_EXPORT WebPoint LocalPoint() const;

  // Coordinates of the point that was hit. Relative to the node, but with
  // ContentBoxOffset removed if the node has box layout.
  BLINK_EXPORT WebPoint LocalPointWithoutContentBoxOffset() const;

  // If a link (eg. anchor or area tag) is hit, return the element.
  // Return null otheriwse.
  BLINK_EXPORT WebElement UrlElement() const;

  // If an image is hit, return the image source. Return empty otherwise.
  BLINK_EXPORT WebURL AbsoluteImageURL() const;

  // If an link is hit, return the link url source. Return empty otherwise.
  BLINK_EXPORT WebURL AbsoluteLinkURL() const;

  // Return whether an editable input element was hit.
  BLINK_EXPORT bool IsContentEditable() const;

#if INSIDE_BLINK
  BLINK_EXPORT WebHitTestResult(const HitTestResult&);
  BLINK_EXPORT WebHitTestResult& operator=(const HitTestResult&);
#endif

 protected:
  WebPrivatePtr<WebHitTestResultPrivate> private_;
};

}  // namespace blink

#endif
