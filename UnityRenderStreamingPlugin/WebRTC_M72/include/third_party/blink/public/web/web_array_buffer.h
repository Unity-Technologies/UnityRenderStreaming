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

#ifndef THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_ARRAY_BUFFER_H_
#define THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_ARRAY_BUFFER_H_

#include "third_party/blink/public/platform/web_common.h"
#include "third_party/blink/public/platform/web_private_ptr.h"

namespace blink {

class DOMArrayBuffer;

class WebArrayBuffer {
 public:
  ~WebArrayBuffer() { Reset(); }

  WebArrayBuffer() = default;
  WebArrayBuffer(const WebArrayBuffer& b) { Assign(b); }
  WebArrayBuffer& operator=(const WebArrayBuffer& b) {
    Assign(b);
    return *this;
  }

  BLINK_EXPORT static WebArrayBuffer Create(unsigned num_elements,
                                            unsigned element_byte_size);

  BLINK_EXPORT void Reset();
  BLINK_EXPORT void Assign(const WebArrayBuffer&);

  bool IsNull() const { return private_.IsNull(); }
  BLINK_EXPORT void* Data() const;
  BLINK_EXPORT unsigned ByteLength() const;

#if INSIDE_BLINK
  BLINK_EXPORT WebArrayBuffer(DOMArrayBuffer*);
  BLINK_EXPORT WebArrayBuffer& operator=(DOMArrayBuffer*);
  BLINK_EXPORT operator DOMArrayBuffer*() const;
#endif

 protected:
  WebPrivatePtr<DOMArrayBuffer> private_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_ARRAY_BUFFER_H_
