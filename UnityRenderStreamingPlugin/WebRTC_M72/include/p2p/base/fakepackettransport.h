/*
 *  Copyright 2017 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef P2P_BASE_FAKEPACKETTRANSPORT_H_
#define P2P_BASE_FAKEPACKETTRANSPORT_H_

#include <string>

#include "api/ortc/packettransportinterface.h"
#include "p2p/base/packettransportinternal.h"
#include "rtc_base/asyncinvoker.h"
#include "rtc_base/copyonwritebuffer.h"

namespace rtc {

// Used to simulate a packet-based transport.
class FakePacketTransport : public PacketTransportInternal {
 public:
  explicit FakePacketTransport(const std::string& transport_name)
      : transport_name_(transport_name) {}
  ~FakePacketTransport() override {
    if (dest_ && dest_->dest_ == this) {
      dest_->dest_ = nullptr;
    }
  }

  // If async, will send packets by "Post"-ing to message queue instead of
  // synchronously "Send"-ing.
  void SetAsync(bool async) { async_ = async; }
  void SetAsyncDelay(int delay_ms) { async_delay_ms_ = delay_ms; }

  // SetWritable, SetReceiving and SetDestination are the main methods that can
  // be used for testing, to simulate connectivity or lack thereof.
  void SetWritable(bool writable) { set_writable(writable); }
  void SetReceiving(bool receiving) { set_receiving(receiving); }

  // Simulates the two transports connecting to each other.
  // If |asymmetric| is true this method only affects this FakePacketTransport.
  // If false, it affects |dest| as well.
  void SetDestination(FakePacketTransport* dest, bool asymmetric) {
    if (dest) {
      dest_ = dest;
      set_writable(true);
      if (!asymmetric) {
        dest->SetDestination(this, true);
      }
    } else {
      // Simulates loss of connectivity, by asymmetrically forgetting dest_.
      dest_ = nullptr;
      set_writable(false);
    }
  }

  // Fake PacketTransportInternal implementation.
  const std::string& transport_name() const override { return transport_name_; }
  bool writable() const override { return writable_; }
  bool receiving() const override { return receiving_; }
  int SendPacket(const char* data,
                 size_t len,
                 const PacketOptions& options,
                 int flags) override {
    if (!dest_) {
      return -1;
    }
    CopyOnWriteBuffer packet(data, len);
    if (async_) {
      invoker_.AsyncInvokeDelayed<void>(
          RTC_FROM_HERE, Thread::Current(),
          Bind(&FakePacketTransport::SendPacketInternal, this, packet),
          async_delay_ms_);
    } else {
      SendPacketInternal(packet);
    }
    SentPacket sent_packet(options.packet_id, TimeMillis());
    SignalSentPacket(this, sent_packet);
    return static_cast<int>(len);
  }
  int SetOption(Socket::Option opt, int value) override { return true; }
  bool GetOption(Socket::Option opt, int* value) override { return true; }
  int GetError() override { return 0; }

  const CopyOnWriteBuffer* last_sent_packet() { return &last_sent_packet_; }

  absl::optional<NetworkRoute> network_route() const override {
    return network_route_;
  }
  void SetNetworkRoute(absl::optional<NetworkRoute> network_route) {
    network_route_ = network_route;
  }

 private:
  void set_writable(bool writable) {
    if (writable_ == writable) {
      return;
    }
    writable_ = writable;
    if (writable_) {
      SignalReadyToSend(this);
    }
    SignalWritableState(this);
  }

  void set_receiving(bool receiving) {
    if (receiving_ == receiving) {
      return;
    }
    receiving_ = receiving;
    SignalReceivingState(this);
  }

  void SendPacketInternal(const CopyOnWriteBuffer& packet) {
    last_sent_packet_ = packet;
    if (dest_) {
      dest_->SignalReadPacket(dest_, packet.data<char>(), packet.size(),
                              TimeMicros(), 0);
    }
  }

  CopyOnWriteBuffer last_sent_packet_;
  AsyncInvoker invoker_;
  std::string transport_name_;
  FakePacketTransport* dest_ = nullptr;
  bool async_ = false;
  int async_delay_ms_ = 0;
  bool writable_ = false;
  bool receiving_ = false;

  absl::optional<NetworkRoute> network_route_;
};

}  // namespace rtc

#endif  // P2P_BASE_FAKEPACKETTRANSPORT_H_
