// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_PEERCONNECTION_RTC_RTP_SENDER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_PEERCONNECTION_RTC_RTP_SENDER_H_

#include "third_party/blink/public/platform/web_rtc_rtp_sender.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise.h"
#include "third_party/blink/renderer/modules/mediastream/media_stream.h"
#include "third_party/blink/renderer/modules/peerconnection/rtc_rtp_encoding_parameters.h"
#include "third_party/blink/renderer/modules/peerconnection/rtc_rtp_send_parameters.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/heap/member.h"
#include "third_party/blink/renderer/platform/heap/visitor.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/webrtc/api/rtptransceiverinterface.h"

namespace blink {

class MediaStreamTrack;
class RTCDTMFSender;
class RTCPeerConnection;
class RTCRtpCapabilities;

webrtc::RtpEncodingParameters ToRtpEncodingParameters(
    const RTCRtpEncodingParameters*);

// https://w3c.github.io/webrtc-pc/#rtcrtpsender-interface
class RTCRtpSender final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  // TODO(hbos): Get rid of sender's reference to RTCPeerConnection?
  // https://github.com/w3c/webrtc-pc/issues/1712
  RTCRtpSender(RTCPeerConnection*,
               std::unique_ptr<WebRTCRtpSender>,
               String kind,
               MediaStreamTrack*,
               MediaStreamVector streams);

  MediaStreamTrack* track();
  ScriptPromise replaceTrack(ScriptState*, MediaStreamTrack*);
  RTCDTMFSender* dtmf();
  static RTCRtpCapabilities* getCapabilities(const String& kind);
  RTCRtpSendParameters* getParameters();
  ScriptPromise setParameters(ScriptState*, const RTCRtpSendParameters*);
  ScriptPromise getStats(ScriptState*);

  WebRTCRtpSender* web_sender();
  // Sets the track. This must be called when the |WebRTCRtpSender| has its
  // track updated, and the |track| must match the |WebRTCRtpSender::Track|.
  void SetTrack(MediaStreamTrack*);
  void ClearLastReturnedParameters();
  MediaStreamVector streams() const;
  void set_streams(MediaStreamVector streams);

  void Trace(blink::Visitor*) override;

 private:
  Member<RTCPeerConnection> pc_;
  std::unique_ptr<WebRTCRtpSender> sender_;
  // The spec says that "kind" should be looked up in transceiver, but keeping
  // a copy here as long as we support Plan B.
  String kind_;
  Member<MediaStreamTrack> track_;
  Member<RTCDTMFSender> dtmf_;
  MediaStreamVector streams_;
  Member<RTCRtpSendParameters> last_returned_parameters_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_PEERCONNECTION_RTC_RTP_SENDER_H_
