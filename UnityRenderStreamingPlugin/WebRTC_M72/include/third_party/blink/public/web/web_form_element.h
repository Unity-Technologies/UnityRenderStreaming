/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_FORM_ELEMENT_H_
#define THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_FORM_ELEMENT_H_

#include "third_party/blink/public/platform/web_vector.h"
#include "third_party/blink/public/web/web_element.h"

namespace blink {

class HTMLFormElement;
class WebFormControlElement;

// A container for passing around a reference to a form element. Provides some
// information about the form.
class BLINK_EXPORT WebFormElement final : public WebElement {
 public:
  ~WebFormElement() override { Reset(); }

  WebFormElement() : WebElement() {}
  WebFormElement(const WebFormElement& element) = default;

  WebFormElement& operator=(const WebFormElement& element) {
    WebElement::Assign(element);
    return *this;
  }
  void Assign(const WebFormElement& element) { WebElement::Assign(element); }

  bool AutoComplete() const;
  WebString Action() const;
  WebString GetName() const;
  WebString Method() const;

  // Returns the identifier which is unique among all form elements in the
  // current renderer process. In the current implementation ids are
  // consecutive numbers so their uniqueness might be broken in case of
  // overflow.
  unsigned UniqueRendererFormId() const;

  void GetFormControlElements(WebVector<WebFormControlElement>&) const;

#if INSIDE_BLINK
  WebFormElement(HTMLFormElement*);
  WebFormElement& operator=(HTMLFormElement*);
  operator HTMLFormElement*() const;
#endif
};

DECLARE_WEB_NODE_TYPE_CASTS(WebFormElement);

}  // namespace blink

#endif
