// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// WebSocketHandshakeThrottle provides a facility for embedders to delay
// WebSocket connection establishment. Specifically, at the same time as the
// handshake is started blink::Platform::CreateWebSocketHandshakeThrottle() will
// be called. If a non-null WebSocketHandshakeThrottle is returned then
// ThrottleHandshake() will be called on it. If the result is error then the
// handshake will be aborted, and a connection error will be reported to
// Javascript. If the throttle hasn't reported a result when the WebSocket
// handshake succeeds then Blink will wait for the throttle result before
// reporting the connection is open to Javascript.

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEBSOCKET_HANDSHAKE_THROTTLE_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEBSOCKET_HANDSHAKE_THROTTLE_H_

#include "third_party/blink/public/platform/web_callbacks.h"

namespace blink {

class WebURL;
class WebString;

// Embedders can implement this class to delay WebSocket connections.
class WebSocketHandshakeThrottle {
 public:
  // Destruction implies that the handshake has been aborted. Any ongoing work
  // should be cleaned up if possible.
  virtual ~WebSocketHandshakeThrottle() = default;

  // The WebCallbacks OnSuccess or OnError should be called asychronously to
  // permit Javascript to use the connection or not. OnError should be passed
  // a message to be displayed on the console indicating why the handshake was
  // blocked. This object will be destroyed synchronously inside the
  // callbacks. Callbacks must not be called after this object has been
  // destroyed.
  virtual void ThrottleHandshake(const WebURL&,
                                 WebCallbacks<void, const WebString&>*) = 0;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEBSOCKET_HANDSHAKE_THROTTLE_H_
