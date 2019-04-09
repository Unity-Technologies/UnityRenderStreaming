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

#ifndef THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_FRAME_SERIALIZER_CLIENT_H_
#define THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_FRAME_SERIALIZER_CLIENT_H_

#include "third_party/blink/public/platform/web_vector.h"

namespace blink {

// This class is used for providing sink interface that can be used to receive
// the individual chunks of data to be saved.
class WebFrameSerializerClient {
 public:
  // This sink interface can receive the individual chunks of serialized data
  // to be saved, so we use values of following enum definition to indicate
  // the serialization status of serializing all html content. If frame is not
  // completely serialized yet, we will call didSerializeDataForFrame with URL
  // of current frame, data, data length and flag CurrentFrameIsNotFinished.
  //
  // If frame is completely serialized, we will call didSerializeDataForFrame
  // with URL of current frame, data, data length and flag
  // CurrentFrameIsFinished.
  enum FrameSerializationStatus {
    kCurrentFrameIsNotFinished,
    kCurrentFrameIsFinished,
  };

  // Receive the individual chunks of serialized and encoded data to be saved.
  // The parameter data contains the available data for saving.
  // The parameter status indicates the status of data serialization.
  virtual void DidSerializeDataForFrame(const WebVector<char>&,
                                        FrameSerializationStatus) = 0;

  WebFrameSerializerClient() = default;

 protected:
  virtual ~WebFrameSerializerClient() = default;
};

}  // namespace blink

#endif
