/*
 *  Copyright 2008 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef P2P_BASE_TESTSTUNSERVER_H_
#define P2P_BASE_TESTSTUNSERVER_H_

#include "p2p/base/stunserver.h"
#include "rtc_base/socketaddress.h"
#include "rtc_base/thread.h"

namespace cricket {

// A test STUN server. Useful for unit tests.
class TestStunServer : StunServer {
 public:
  static TestStunServer* Create(rtc::Thread* thread,
                                const rtc::SocketAddress& addr);

  // Set a fake STUN address to return to the client.
  void set_fake_stun_addr(const rtc::SocketAddress& addr) {
    fake_stun_addr_ = addr;
  }

 private:
  explicit TestStunServer(rtc::AsyncUDPSocket* socket) : StunServer(socket) {}

  void OnBindingRequest(StunMessage* msg,
                        const rtc::SocketAddress& remote_addr) override;

 private:
  rtc::SocketAddress fake_stun_addr_;
};

}  // namespace cricket

#endif  // P2P_BASE_TESTSTUNSERVER_H_
