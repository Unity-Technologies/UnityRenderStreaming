/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_RTPTRANSPORTINTERNALADAPTER_H_
#define PC_RTPTRANSPORTINTERNALADAPTER_H_

#include <memory>
#include <utility>

#include "pc/rtptransportinternal.h"

namespace webrtc {

// This class is used by SrtpTransport and DtlsSrtpTransport in order to reduce
// the duplicated code. Using this class, different subclasses can override only
// part of RtpTransportInternal methods without implementing all the common
// methods.
class RtpTransportInternalAdapter : public RtpTransportInternal {
 public:
  RtpTransportInternalAdapter() {}

  explicit RtpTransportInternalAdapter(RtpTransportInternal* transport)
      : transport_(transport) {
    RTC_DCHECK(transport_);
  }

  void SetRtcpMuxEnabled(bool enable) override {
    transport_->SetRtcpMuxEnabled(enable);
  }

  bool rtcp_mux_enabled() const override {
    return transport_->rtcp_mux_enabled();
  }

  rtc::PacketTransportInternal* rtp_packet_transport() const override {
    return transport_->rtp_packet_transport();
  }
  void SetRtpPacketTransport(rtc::PacketTransportInternal* rtp) override {
    transport_->SetRtpPacketTransport(rtp);
  }

  rtc::PacketTransportInternal* rtcp_packet_transport() const override {
    return transport_->rtcp_packet_transport();
  }
  void SetRtcpPacketTransport(rtc::PacketTransportInternal* rtcp) override {
    transport_->SetRtcpPacketTransport(rtcp);
  }

  bool IsReadyToSend() const override { return transport_->IsReadyToSend(); }

  bool IsWritable(bool rtcp) const override {
    return transport_->IsWritable(rtcp);
  }

  bool SendRtpPacket(rtc::CopyOnWriteBuffer* packet,
                     const rtc::PacketOptions& options,
                     int flags) override {
    return transport_->SendRtpPacket(packet, options, flags);
  }

  bool SendRtcpPacket(rtc::CopyOnWriteBuffer* packet,
                      const rtc::PacketOptions& options,
                      int flags) override {
    return transport_->SendRtcpPacket(packet, options, flags);
  }

  void UpdateRtpHeaderExtensionMap(
      const cricket::RtpHeaderExtensions& header_extensions) override {
    transport_->UpdateRtpHeaderExtensionMap(header_extensions);
  }

  bool RegisterRtpDemuxerSink(const RtpDemuxerCriteria& criteria,
                              RtpPacketSinkInterface* sink) override {
    return transport_->RegisterRtpDemuxerSink(criteria, sink);
  }

  bool UnregisterRtpDemuxerSink(RtpPacketSinkInterface* sink) override {
    return transport_->UnregisterRtpDemuxerSink(sink);
  }

  // RtpTransportInterface overrides.
  PacketTransportInterface* GetRtpPacketTransport() const override {
    return transport_->GetRtpPacketTransport();
  }

  PacketTransportInterface* GetRtcpPacketTransport() const override {
    return transport_->GetRtcpPacketTransport();
  }

  RTCError SetParameters(const RtpTransportParameters& parameters) override {
    return transport_->SetParameters(parameters);
  }

  RtpTransportParameters GetParameters() const override {
    return transport_->GetParameters();
  }

  RtpTransportAdapter* GetInternal() override { return nullptr; }

 protected:
  // Owned by the subclasses.
  RtpTransportInternal* transport_ = nullptr;
};

}  // namespace webrtc

#endif  // PC_RTPTRANSPORTINTERNALADAPTER_H_
