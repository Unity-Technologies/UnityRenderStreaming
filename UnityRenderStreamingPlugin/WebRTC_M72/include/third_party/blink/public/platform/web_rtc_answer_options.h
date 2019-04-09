// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_RTC_ANSWER_OPTIONS_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_RTC_ANSWER_OPTIONS_H_

#include "third_party/blink/public/platform/web_common.h"
#include "third_party/blink/public/platform/web_private_ptr.h"

namespace blink {

class RTCAnswerOptionsPlatform;

class BLINK_PLATFORM_EXPORT WebRTCAnswerOptions {
 public:
  WebRTCAnswerOptions(const WebRTCAnswerOptions& other) { Assign(other); }
  ~WebRTCAnswerOptions() { Reset(); }

  WebRTCAnswerOptions& operator=(const WebRTCAnswerOptions& other) {
    Assign(other);
    return *this;
  }

  void Assign(const WebRTCAnswerOptions&);

  void Reset();
  bool IsNull() const { return private_.IsNull(); }

  bool VoiceActivityDetection() const;

#if INSIDE_BLINK
  WebRTCAnswerOptions(RTCAnswerOptionsPlatform*);
#endif

 private:
  WebPrivatePtr<RTCAnswerOptionsPlatform> private_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_RTC_ANSWER_OPTIONS_H_
