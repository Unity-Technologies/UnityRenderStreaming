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

#ifndef THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_DOM_MEDIA_STREAM_TRACK_H_
#define THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_DOM_MEDIA_STREAM_TRACK_H_

#include "third_party/blink/public/platform/web_media_stream_track.h"
#include "third_party/blink/public/platform/web_private_ptr.h"

namespace v8 {
class Value;
template <class T>
class Local;
}

namespace blink {

class MediaStreamTrack;

class WebDOMMediaStreamTrack {
 public:
  WebDOMMediaStreamTrack(const WebDOMMediaStreamTrack& b) { Assign(b); }
  ~WebDOMMediaStreamTrack() { Reset(); }

  WebDOMMediaStreamTrack& operator=(const WebDOMMediaStreamTrack& b) {
    Assign(b);
    return *this;
  }

  bool IsNull() const { return private_.IsNull(); }

  BLINK_EXPORT static WebDOMMediaStreamTrack FromV8Value(v8::Local<v8::Value>);

  BLINK_EXPORT void Reset();
  BLINK_EXPORT void Assign(const WebDOMMediaStreamTrack&);
  BLINK_EXPORT WebMediaStreamTrack Component() const;

 private:
#if INSIDE_BLINK
  WebDOMMediaStreamTrack(MediaStreamTrack*);
#endif

  WebPrivatePtr<MediaStreamTrack> private_;
};

}  // namespace blink

#endif
