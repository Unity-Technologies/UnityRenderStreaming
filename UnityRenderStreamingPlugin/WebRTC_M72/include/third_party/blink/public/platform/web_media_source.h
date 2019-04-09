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

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_MEDIA_SOURCE_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_MEDIA_SOURCE_H_

#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/platform/web_time_range.h"
#include "third_party/blink/public/platform/web_url.h"

namespace blink {

class WebSourceBuffer;

class WebMediaSource {
 public:
  enum AddStatus {
    kAddStatusOk,
    kAddStatusNotSupported,
    kAddStatusReachedIdLimit
  };

  enum EndOfStreamStatus {
    kEndOfStreamStatusNoError,
    kEndOfStreamStatusNetworkError,
    kEndOfStreamStatusDecodeError,
  };

  virtual ~WebMediaSource() = default;

  // Attempts to create a new WebSourceBuffer for use with this WebMediaSource.
  // |content_type| is the ContentType string of the new WebSourceBuffer
  // bytestream's MIME type, and |codecs| contains the "codecs" parameter
  // string, if any, of the bytestream's MIME type.
  // If this WebMediaSource supports the format indicated by |content_type| and
  // |codecs| and has enough resources to support a new WebSourceBuffer, returns
  // kAddStatusOk and creates a new WebSourceBuffer and changes the
  // WebSourceBuffer parameter to point to it.
  // If this WebMediaSource cannot handle another WebSourceBuffer right now,
  // returns kAddStatusReachedIdLimit without modifying the WebSourceBuffer
  // parameter.
  // If |content_type| and |codecs| are not supported, returns
  // kAddStatusNotSupported without modifying the WebSourceBuffer parameter.
  virtual AddStatus AddSourceBuffer(const WebString& content_type,
                                    const WebString& codecs,
                                    WebSourceBuffer**) = 0;

  virtual double Duration() = 0;
  virtual void SetDuration(double) = 0;
  virtual void MarkEndOfStream(EndOfStreamStatus) = 0;
  virtual void UnmarkEndOfStream() = 0;
};

}  // namespace blink

#endif
