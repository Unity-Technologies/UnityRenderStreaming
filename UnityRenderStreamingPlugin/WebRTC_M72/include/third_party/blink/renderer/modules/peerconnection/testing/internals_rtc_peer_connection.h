// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_PEERCONNECTION_TESTING_INTERNALS_RTC_PEER_CONNECTION_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_PEERCONNECTION_TESTING_INTERNALS_RTC_PEER_CONNECTION_H_

#include "third_party/blink/renderer/modules/peerconnection/rtc_peer_connection.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace blink {

class Internals;

class InternalsRTCPeerConnection {
  STATIC_ONLY(InternalsRTCPeerConnection);

 public:
  static int peerConnectionCount(Internals&);
  static int peerConnectionCountLimit(Internals&);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_PEERCONNECTION_TESTING_INTERNALS_RTC_PEER_CONNECTION_H_
