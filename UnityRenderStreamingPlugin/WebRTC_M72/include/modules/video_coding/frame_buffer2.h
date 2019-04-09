/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CODING_FRAME_BUFFER2_H_
#define MODULES_VIDEO_CODING_FRAME_BUFFER2_H_

#include <array>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "api/video/encoded_frame.h"
#include "modules/video_coding/include/video_coding_defines.h"
#include "modules/video_coding/inter_frame_delay.h"
#include "rtc_base/constructormagic.h"
#include "rtc_base/criticalsection.h"
#include "rtc_base/event.h"
#include "rtc_base/experiments/rtt_mult_experiment.h"
#include "rtc_base/numerics/sequence_number_util.h"
#include "rtc_base/thread_annotations.h"

namespace webrtc {

class Clock;
class VCMReceiveStatisticsCallback;
class VCMJitterEstimator;
class VCMTiming;

namespace video_coding {

class FrameBuffer {
 public:
  enum ReturnReason { kFrameFound, kTimeout, kStopped };

  FrameBuffer(Clock* clock,
              VCMJitterEstimator* jitter_estimator,
              VCMTiming* timing,
              VCMReceiveStatisticsCallback* stats_proxy);

  virtual ~FrameBuffer();

  // Insert a frame into the frame buffer. Returns the picture id
  // of the last continuous frame or -1 if there is no continuous frame.
  // TODO(philipel): Return a VideoLayerFrameId and not only the picture id.
  int64_t InsertFrame(std::unique_ptr<EncodedFrame> frame);

  // Get the next frame for decoding. Will return at latest after
  // |max_wait_time_ms|.
  //  - If a frame is available within |max_wait_time_ms| it will return
  //    kFrameFound and set |frame_out| to the resulting frame.
  //  - If no frame is available after |max_wait_time_ms| it will return
  //    kTimeout.
  //  - If the FrameBuffer is stopped then it will return kStopped.
  ReturnReason NextFrame(int64_t max_wait_time_ms,
                         std::unique_ptr<EncodedFrame>* frame_out,
                         bool keyframe_required = false);

  // Tells the FrameBuffer which protection mode that is in use. Affects
  // the frame timing.
  // TODO(philipel): Remove this when new timing calculations has been
  //                 implemented.
  void SetProtectionMode(VCMVideoProtection mode);

  // Start the frame buffer, has no effect if the frame buffer is started.
  // The frame buffer is started upon construction.
  void Start();

  // Stop the frame buffer, causing any sleeping thread in NextFrame to
  // return immediately.
  void Stop();

  // Updates the RTT for jitter buffer estimation.
  void UpdateRtt(int64_t rtt_ms);

  // Clears the FrameBuffer, removing all the buffered frames.
  void Clear();

 private:
  struct FrameInfo {
    FrameInfo();
    FrameInfo(FrameInfo&&);
    ~FrameInfo();

    // The maximum number of frames that can depend on this frame.
    static constexpr size_t kMaxNumDependentFrames = 8;

    // Which other frames that have direct unfulfilled dependencies
    // on this frame.
    // TODO(philipel): Add simple modify/access functions to prevent adding too
    // many |dependent_frames|.
    VideoLayerFrameId dependent_frames[kMaxNumDependentFrames];
    size_t num_dependent_frames = 0;

    // A frame is continiuous if it has all its referenced/indirectly
    // referenced frames.
    //
    // How many unfulfilled frames this frame have until it becomes continuous.
    size_t num_missing_continuous = 0;

    // A frame is decodable if all its referenced frames have been decoded.
    //
    // How many unfulfilled frames this frame have until it becomes decodable.
    size_t num_missing_decodable = 0;

    // If this frame is continuous or not.
    bool continuous = false;

    // The actual EncodedFrame.
    std::unique_ptr<EncodedFrame> frame;
  };

  using FrameMap = std::map<VideoLayerFrameId, FrameInfo>;

  // Check that the references of |frame| are valid.
  bool ValidReferences(const EncodedFrame& frame) const;

  // Updates the minimal and maximal playout delays
  // depending on the frame.
  void UpdatePlayoutDelays(const EncodedFrame& frame)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(crit_);

  // Update all directly dependent and indirectly dependent frames and mark
  // them as continuous if all their references has been fulfilled.
  void PropagateContinuity(FrameMap::iterator start)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(crit_);

  // Marks the frame as decoded and updates all directly dependent frames.
  void PropagateDecodability(const FrameInfo& info)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(crit_);

  // Advances |last_decoded_frame_it_| to |decoded| and removes old
  // frame info.
  void AdvanceLastDecodedFrame(FrameMap::iterator decoded)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(crit_);

  // Update the corresponding FrameInfo of |frame| and all FrameInfos that
  // |frame| references.
  // Return false if |frame| will never be decodable, true otherwise.
  bool UpdateFrameInfoWithIncomingFrame(const EncodedFrame& frame,
                                        FrameMap::iterator info)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(crit_);

  void UpdateJitterDelay() RTC_EXCLUSIVE_LOCKS_REQUIRED(crit_);

  void UpdateTimingFrameInfo() RTC_EXCLUSIVE_LOCKS_REQUIRED(crit_);

  void ClearFramesAndHistory() RTC_EXCLUSIVE_LOCKS_REQUIRED(crit_);

  bool HasBadRenderTiming(const EncodedFrame& frame, int64_t now_ms)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(crit_);

  // The cleaner solution would be to have the NextFrame function return a
  // vector of frames, but until the decoding pipeline can support decoding
  // multiple frames at the same time we combine all frames to one frame and
  // return it. See bugs.webrtc.org/10064
  EncodedFrame* CombineAndDeleteFrames(
      const std::vector<EncodedFrame*>& frames) const;

  FrameMap frames_ RTC_GUARDED_BY(crit_);

  rtc::CriticalSection crit_;
  Clock* const clock_;
  rtc::Event new_continuous_frame_event_;
  VCMJitterEstimator* const jitter_estimator_ RTC_GUARDED_BY(crit_);
  VCMTiming* const timing_ RTC_GUARDED_BY(crit_);
  VCMInterFrameDelay inter_frame_delay_ RTC_GUARDED_BY(crit_);
  absl::optional<uint32_t> last_decoded_frame_timestamp_ RTC_GUARDED_BY(crit_);
  FrameMap::iterator last_decoded_frame_it_ RTC_GUARDED_BY(crit_);
  FrameMap::iterator last_continuous_frame_it_ RTC_GUARDED_BY(crit_);
  std::vector<FrameMap::iterator> frames_to_decode_ RTC_GUARDED_BY(crit_);
  int num_frames_history_ RTC_GUARDED_BY(crit_);
  int num_frames_buffered_ RTC_GUARDED_BY(crit_);
  bool stopped_ RTC_GUARDED_BY(crit_);
  VCMVideoProtection protection_mode_ RTC_GUARDED_BY(crit_);
  VCMReceiveStatisticsCallback* const stats_callback_;
  int64_t last_log_non_decoded_ms_ RTC_GUARDED_BY(crit_);

  RTC_DISALLOW_IMPLICIT_CONSTRUCTORS(FrameBuffer);
};

}  // namespace video_coding
}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_FRAME_BUFFER2_H_
