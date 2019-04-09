/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_CODING_INCLUDE_AUDIO_CODING_MODULE_TYPEDEFS_H_
#define MODULES_AUDIO_CODING_INCLUDE_AUDIO_CODING_MODULE_TYPEDEFS_H_

#include <map>

#include "rtc_base/deprecation.h"

namespace webrtc {

///////////////////////////////////////////////////////////////////////////
// enum ACMVADMode
// An enumerator for aggressiveness of VAD
// -VADNormal                : least aggressive mode.
// -VADLowBitrate            : more aggressive than "VADNormal" to save on
//                             bit-rate.
// -VADAggr                  : an aggressive mode.
// -VADVeryAggr              : the most agressive mode.
//
enum ACMVADMode {
  VADNormal = 0,
  VADLowBitrate = 1,
  VADAggr = 2,
  VADVeryAggr = 3
};

///////////////////////////////////////////////////////////////////////////
//
// Enumeration of Opus mode for intended application.
//
// kVoip              : optimized for voice signals.
// kAudio             : optimized for non-voice signals like music.
//
enum OpusApplicationMode {
  kVoip = 0,
  kAudio = 1,
};

// Statistics for calls to AudioCodingModule::PlayoutData10Ms().
struct AudioDecodingCallStats {
  AudioDecodingCallStats()
      : calls_to_silence_generator(0),
        calls_to_neteq(0),
        decoded_normal(0),
        decoded_plc(0),
        decoded_cng(0),
        decoded_plc_cng(0),
        decoded_muted_output(0) {}

  int calls_to_silence_generator;  // Number of calls where silence generated,
                                   // and NetEq was disengaged from decoding.
  int calls_to_neteq;              // Number of calls to NetEq.
  int decoded_normal;  // Number of calls where audio RTP packet decoded.
  int decoded_plc;     // Number of calls resulted in PLC.
  int decoded_cng;  // Number of calls where comfort noise generated due to DTX.
  int decoded_plc_cng;       // Number of calls resulted where PLC faded to CNG.
  int decoded_muted_output;  // Number of calls returning a muted state output.
};

// NETEQ statistics.
struct NetworkStatistics {
  // current jitter buffer size in ms
  uint16_t currentBufferSize;
  // preferred (optimal) buffer size in ms
  uint16_t preferredBufferSize;
  // adding extra delay due to "peaky jitter"
  bool jitterPeaksFound;
  // Stats below correspond to similarly-named fields in the WebRTC stats spec.
  // https://w3c.github.io/webrtc-stats/#dom-rtcmediastreamtrackstats
  uint64_t totalSamplesReceived;
  uint64_t concealedSamples;
  uint64_t concealmentEvents;
  uint64_t jitterBufferDelayMs;
  // Stats below DO NOT correspond directly to anything in the WebRTC stats
  // Loss rate (network + late); fraction between 0 and 1, scaled to Q14.
  uint16_t currentPacketLossRate;
  // Late loss rate; fraction between 0 and 1, scaled to Q14.
  union {
    RTC_DEPRECATED uint16_t currentDiscardRate;
  };
  // fraction (of original stream) of synthesized audio inserted through
  // expansion (in Q14)
  uint16_t currentExpandRate;
  // fraction (of original stream) of synthesized speech inserted through
  // expansion (in Q14)
  uint16_t currentSpeechExpandRate;
  // fraction of synthesized speech inserted through pre-emptive expansion
  // (in Q14)
  uint16_t currentPreemptiveRate;
  // fraction of data removed through acceleration (in Q14)
  uint16_t currentAccelerateRate;
  // fraction of data coming from secondary decoding (in Q14)
  uint16_t currentSecondaryDecodedRate;
  // Fraction of secondary data, including FEC and RED, that is discarded (in
  // Q14). Discarding of secondary data can be caused by the reception of the
  // primary data, obsoleting the secondary data. It can also be caused by early
  // or late arrival of secondary data.
  uint16_t currentSecondaryDiscardedRate;
  // clock-drift in parts-per-million (negative or positive)
  int32_t clockDriftPPM;
  // average packet waiting time in the jitter buffer (ms)
  int meanWaitingTimeMs;
  // median packet waiting time in the jitter buffer (ms)
  int medianWaitingTimeMs;
  // min packet waiting time in the jitter buffer (ms)
  int minWaitingTimeMs;
  // max packet waiting time in the jitter buffer (ms)
  int maxWaitingTimeMs;
  // added samples in off mode due to packet loss
  size_t addedSamples;
  // count of the number of buffer flushes
  uint64_t packetBufferFlushes;
  // number of samples expanded due to delayed packets
  uint64_t delayedPacketOutageSamples;
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_CODING_INCLUDE_AUDIO_CODING_MODULE_TYPEDEFS_H_
