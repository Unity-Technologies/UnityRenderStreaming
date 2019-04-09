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

#ifndef THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_CUSTOM_ELEMENT_H_
#define THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_CUSTOM_ELEMENT_H_

#include "base/macros.h"
#include "third_party/blink/public/platform/web_common.h"

namespace blink {

class WebString;

class WebCustomElement {
 public:
  // Adds a name to the set of names embedders can use with
  // window.customElements.define to register their own types. Because Custom
  // Element processing requires the set of valid names to be known ahead of
  // time, this method should be called before an element with this name is
  // created.
  BLINK_EXPORT static void AddEmbedderCustomElementName(const WebString& name);

  // Allows the use of names added with |AddEmbedderCustomElementName| during
  // window.customElements.define.
  class BLINK_EXPORT EmbedderNamesAllowedScope {
   public:
    EmbedderNamesAllowedScope();
    ~EmbedderNamesAllowedScope();

    static bool IsAllowed();

   private:
    DISALLOW_COPY_AND_ASSIGN(EmbedderNamesAllowedScope);
    void* operator new(size_t) = delete;
  };

 private:
  WebCustomElement() = delete;
};

}  // namespace blink

#endif
