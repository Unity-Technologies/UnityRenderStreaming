/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef CALL_VIDEO_RECEIVE_STREAM_H_
#define CALL_VIDEO_RECEIVE_STREAM_H_

#include <limits>
#include <map>
#include <string>
#include <vector>

#include "api/call/transport.h"
#include "api/crypto/cryptooptions.h"
#include "api/rtp_headers.h"
#include "api/rtpparameters.h"
#include "api/rtpreceiverinterface.h"
#include "api/video/video_content_type.h"
#include "api/video/video_sink_interface.h"
#include "api/video/video_timing.h"
#include "api/video_codecs/sdp_video_format.h"
#include "call/rtp_config.h"
#include "modules/rtp_rtcp/include/rtp_rtcp_defines.h"

namespace webrtc {

class FrameDecryptorInterface;
class RtpPacketSinkInterface;
class VideoDecoderFactory;

class VideoReceiveStream {
 public:
  // TODO(mflodman) Move all these settings to VideoDecoder and move the
  // declaration to common_types.h.
  struct Decoder {
    Decoder();
    Decoder(const Decoder&);
    ~Decoder();
    std::string ToString() const;

    // Ownership stays with WebrtcVideoEngine (delegated from PeerConnection).
    // TODO(nisse): Move one level out, to VideoReceiveStream::Config, and later
    // to the configuration of VideoStreamDecoder.
    VideoDecoderFactory* decoder_factory = nullptr;
    SdpVideoFormat video_format;

    // Received RTP packets with this payload type will be sent to this decoder
    // instance.
    int payload_type = 0;
  };

  struct Stats {
    Stats();
    ~Stats();
    std::string ToString(int64_t time_ms) const;

    int network_frame_rate = 0;
    int decode_frame_rate = 0;
    int render_frame_rate = 0;
    uint32_t frames_rendered = 0;

    // Decoder stats.
    std::string decoder_implementation_name = "unknown";
    FrameCounts frame_counts;
    int decode_ms = 0;
    int max_decode_ms = 0;
    int current_delay_ms = 0;
    int target_delay_ms = 0;
    int jitter_buffer_ms = 0;
    int min_playout_delay_ms = 0;
    int render_delay_ms = 10;
    int64_t interframe_delay_max_ms = -1;
    uint32_t frames_decoded = 0;
    absl::optional<uint64_t> qp_sum;

    int current_payload_type = -1;

    int total_bitrate_bps = 0;
    int discarded_packets = 0;

    int width = 0;
    int height = 0;

    VideoContentType content_type = VideoContentType::UNSPECIFIED;

    int sync_offset_ms = std::numeric_limits<int>::max();

    uint32_t ssrc = 0;
    std::string c_name;
    StreamDataCounters rtp_stats;
    RtcpPacketTypeCounter rtcp_packet_type_counts;
    RtcpStatistics rtcp_stats;

    // Timing frame info: all important timestamps for a full lifetime of a
    // single 'timing frame'.
    absl::optional<webrtc::TimingFrameInfo> timing_frame_info;
  };

  struct Config {
   private:
    // Access to the copy constructor is private to force use of the Copy()
    // method for those exceptional cases where we do use it.
    Config(const Config&);

   public:
    Config() = delete;
    Config(Config&&);
    explicit Config(Transport* rtcp_send_transport);
    Config& operator=(Config&&);
    Config& operator=(const Config&) = delete;
    ~Config();

    // Mostly used by tests.  Avoid creating copies if you can.
    Config Copy() const { return Config(*this); }

    std::string ToString() const;

    // Decoders for every payload that we can receive.
    std::vector<Decoder> decoders;

    // Receive-stream specific RTP settings.
    struct Rtp {
      Rtp();
      Rtp(const Rtp&);
      ~Rtp();
      std::string ToString() const;

      // Synchronization source (stream identifier) to be received.
      uint32_t remote_ssrc = 0;

      // Sender SSRC used for sending RTCP (such as receiver reports).
      uint32_t local_ssrc = 0;

      // See RtcpMode for description.
      RtcpMode rtcp_mode = RtcpMode::kCompound;

      // Extended RTCP settings.
      struct RtcpXr {
        // True if RTCP Receiver Reference Time Report Block extension
        // (RFC 3611) should be enabled.
        bool receiver_reference_time_report = false;
      } rtcp_xr;

      // TODO(nisse): This remb setting is currently set but never
      // applied. REMB logic is now the responsibility of
      // PacketRouter, and it will generate REMB feedback if
      // OnReceiveBitrateChanged is used, which depends on how the
      // estimators belonging to the ReceiveSideCongestionController
      // are configured. Decide if this setting should be deleted, and
      // if it needs to be replaced by a setting in PacketRouter to
      // disable REMB feedback.

      // See draft-alvestrand-rmcat-remb for information.
      bool remb = false;

      // See draft-holmer-rmcat-transport-wide-cc-extensions for details.
      bool transport_cc = false;

      // See NackConfig for description.
      NackConfig nack;

      // Payload types for ULPFEC and RED, respectively.
      int ulpfec_payload_type = -1;
      int red_payload_type = -1;

      // SSRC for retransmissions.
      uint32_t rtx_ssrc = 0;

      // Set if the stream is protected using FlexFEC.
      bool protected_by_flexfec = false;

      // Map from rtx payload type -> media payload type.
      // For RTX to be enabled, both an SSRC and this mapping are needed.
      std::map<int, int> rtx_associated_payload_types;
      // TODO(nisse): This is a temporary accessor function to enable
      // reversing and renaming of the rtx_payload_types mapping.
      void AddRtxBinding(int rtx_payload_type, int media_payload_type) {
        rtx_associated_payload_types[rtx_payload_type] = media_payload_type;
      }

      // RTP header extensions used for the received stream.
      std::vector<RtpExtension> extensions;
    } rtp;

    // Transport for outgoing packets (RTCP).
    Transport* rtcp_send_transport = nullptr;

    // Must not be 'nullptr' when the stream is started.
    rtc::VideoSinkInterface<VideoFrame>* renderer = nullptr;

    // Expected delay needed by the renderer, i.e. the frame will be delivered
    // this many milliseconds, if possible, earlier than the ideal render time.
    // Only valid if 'renderer' is set.
    int render_delay_ms = 10;

    // If set, pass frames on to the renderer as soon as they are
    // available.
    bool disable_prerenderer_smoothing = false;

    // Identifier for an A/V synchronization group. Empty string to disable.
    // TODO(pbos): Synchronize streams in a sync group, not just video streams
    // to one of the audio streams.
    std::string sync_group;

    // Target delay in milliseconds. A positive value indicates this stream is
    // used for streaming instead of a real-time call.
    int target_delay_ms = 0;

    // TODO(nisse): Used with VideoDecoderFactory::LegacyCreateVideoDecoder.
    // Delete when that method is retired.
    std::string stream_id;

    // An optional custom frame decryptor that allows the entire frame to be
    // decrypted in whatever way the caller choses. This is not required by
    // default.
    rtc::scoped_refptr<webrtc::FrameDecryptorInterface> frame_decryptor;

    // Per PeerConnection cryptography options.
    CryptoOptions crypto_options;
  };

  // Starts stream activity.
  // When a stream is active, it can receive, process and deliver packets.
  virtual void Start() = 0;
  // Stops stream activity.
  // When a stream is stopped, it can't receive, process or deliver packets.
  virtual void Stop() = 0;

  // TODO(pbos): Add info on currently-received codec to Stats.
  virtual Stats GetStats() const = 0;

  // RtpDemuxer only forwards a given RTP packet to one sink. However, some
  // sinks, such as FlexFEC, might wish to be informed of all of the packets
  // a given sink receives (or any set of sinks). They may do so by registering
  // themselves as secondary sinks.
  virtual void AddSecondarySink(RtpPacketSinkInterface* sink) = 0;
  virtual void RemoveSecondarySink(const RtpPacketSinkInterface* sink) = 0;

  virtual std::vector<RtpSource> GetSources() const = 0;

 protected:
  virtual ~VideoReceiveStream() {}
};

}  // namespace webrtc

#endif  // CALL_VIDEO_RECEIVE_STREAM_H_
