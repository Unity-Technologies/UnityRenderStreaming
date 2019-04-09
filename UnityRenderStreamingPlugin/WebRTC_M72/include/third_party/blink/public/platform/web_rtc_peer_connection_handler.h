/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_RTC_PEER_CONNECTION_HANDLER_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_RTC_PEER_CONNECTION_HANDLER_H_

#include "third_party/blink/public/platform/web_rtc_ice_candidate.h"
#include "third_party/blink/public/platform/web_rtc_rtp_transceiver.h"
#include "third_party/blink/public/platform/web_rtc_stats.h"
#include "third_party/blink/public/platform/web_vector.h"
#include "third_party/webrtc/api/peerconnectioninterface.h"
#include "third_party/webrtc/api/rtcerror.h"
#include "third_party/webrtc/api/rtptransceiverinterface.h"

namespace webrtc {
enum class RTCErrorType;
}

namespace blink {

class WebMediaConstraints;
class WebMediaStream;
class WebMediaStreamTrack;
class WebRTCAnswerOptions;
class WebRTCDataChannelHandler;
class WebRTCOfferOptions;
class WebRTCRtpSender;
class WebRTCSessionDescription;
class WebRTCSessionDescriptionRequest;
class WebRTCStatsRequest;
class WebRTCVoidRequest;
class WebString;
struct WebRTCDataChannelInit;

class WebRTCPeerConnectionHandler {
 public:
  virtual ~WebRTCPeerConnectionHandler() = default;

  virtual bool Initialize(
      const webrtc::PeerConnectionInterface::RTCConfiguration&,
      const WebMediaConstraints&) = 0;

  virtual void CreateOffer(const WebRTCSessionDescriptionRequest&,
                           const WebMediaConstraints&) = 0;
  virtual void CreateOffer(const WebRTCSessionDescriptionRequest&,
                           const WebRTCOfferOptions&) = 0;
  virtual void CreateAnswer(const WebRTCSessionDescriptionRequest&,
                            const WebMediaConstraints&) = 0;
  virtual void CreateAnswer(const WebRTCSessionDescriptionRequest&,
                            const WebRTCAnswerOptions&) = 0;
  virtual void SetLocalDescription(const WebRTCVoidRequest&,
                                   const WebRTCSessionDescription&) = 0;
  virtual void SetRemoteDescription(const WebRTCVoidRequest&,
                                    const WebRTCSessionDescription&) = 0;
  virtual WebRTCSessionDescription LocalDescription() = 0;
  virtual WebRTCSessionDescription RemoteDescription() = 0;
  virtual WebRTCSessionDescription CurrentLocalDescription() = 0;
  virtual WebRTCSessionDescription CurrentRemoteDescription() = 0;
  virtual WebRTCSessionDescription PendingLocalDescription() = 0;
  virtual WebRTCSessionDescription PendingRemoteDescription() = 0;
  virtual const webrtc::PeerConnectionInterface::RTCConfiguration&
  GetConfiguration() const = 0;
  virtual webrtc::RTCErrorType SetConfiguration(
      const webrtc::PeerConnectionInterface::RTCConfiguration&) = 0;

  // DEPRECATED
  virtual bool AddICECandidate(scoped_refptr<WebRTCICECandidate>) {
    return false;
  }

  virtual bool AddICECandidate(const WebRTCVoidRequest&,
                               scoped_refptr<WebRTCICECandidate>) {
    return false;
  }
  virtual void GetStats(const WebRTCStatsRequest&) = 0;
  // Gets stats using the new stats collection API, see
  // third_party/webrtc/api/stats/.  These will replace the old stats collection
  // API when the new API has matured enough.
  virtual void GetStats(std::unique_ptr<WebRTCStatsReportCallback>) = 0;
  virtual WebRTCDataChannelHandler* CreateDataChannel(
      const WebString& label,
      const WebRTCDataChannelInit&) = 0;
  virtual webrtc::RTCErrorOr<std::unique_ptr<WebRTCRtpTransceiver>>
  AddTransceiverWithTrack(const WebMediaStreamTrack&,
                          const webrtc::RtpTransceiverInit&) = 0;
  virtual webrtc::RTCErrorOr<std::unique_ptr<WebRTCRtpTransceiver>>
  AddTransceiverWithKind(
      // webrtc::MediaStreamTrackInterface::kAudioKind or kVideoKind
      std::string kind,
      const webrtc::RtpTransceiverInit&) = 0;
  // Adds the track to the peer connection, returning the resulting transceiver
  // or error.
  virtual webrtc::RTCErrorOr<std::unique_ptr<WebRTCRtpTransceiver>> AddTrack(
      const WebMediaStreamTrack&,
      const WebVector<WebMediaStream>&) = 0;
  // Removes the sender.
  // In Plan B: Returns OK() with value nullptr on success. The sender's track
  // must be nulled by the caller.
  // In Unified Plan: Returns OK() with the updated transceiver state.
  virtual webrtc::RTCErrorOr<std::unique_ptr<WebRTCRtpTransceiver>> RemoveTrack(
      WebRTCRtpSender*) = 0;
  virtual void Stop() = 0;

  // Origin Trial - RtcPeerConnectionId
  virtual WebString Id() const = 0;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_RTC_PEER_CONNECTION_HANDLER_H_
