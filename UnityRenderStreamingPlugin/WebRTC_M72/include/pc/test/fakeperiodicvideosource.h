/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_TEST_FAKEPERIODICVIDEOSOURCE_H_
#define PC_TEST_FAKEPERIODICVIDEOSOURCE_H_

#include <memory>

#include "absl/memory/memory.h"
#include "api/video/video_source_interface.h"
#include "media/base/fakeframesource.h"
#include "media/base/videobroadcaster.h"
#include "rtc_base/task_queue.h"

namespace webrtc {

class FakePeriodicVideoSource final
    : public rtc::VideoSourceInterface<VideoFrame> {
 public:
  static constexpr int kDefaultFrameIntervalMs = 33;
  static constexpr int kDefaultWidth = 640;
  static constexpr int kDefaultHeight = 480;

  struct Config {
    int width = kDefaultWidth;
    int height = kDefaultHeight;
    int frame_interval_ms = kDefaultFrameIntervalMs;
    VideoRotation rotation = kVideoRotation_0;
    int64_t timestamp_offset_ms = 0;
  };

  FakePeriodicVideoSource() : FakePeriodicVideoSource(Config()) {}
  explicit FakePeriodicVideoSource(Config config)
      : task_queue_(
            absl::make_unique<rtc::TaskQueue>("FakePeriodicVideoTrackSource")) {
    thread_checker_.DetachFromThread();
    task_queue_->PostTask(absl::make_unique<FrameTask>(config, &broadcaster_));
  }

  void RemoveSink(rtc::VideoSinkInterface<webrtc::VideoFrame>* sink) override {
    RTC_DCHECK(thread_checker_.CalledOnValidThread());
    broadcaster_.RemoveSink(sink);
  }

  void AddOrUpdateSink(rtc::VideoSinkInterface<webrtc::VideoFrame>* sink,
                       const rtc::VideoSinkWants& wants) override {
    RTC_DCHECK(thread_checker_.CalledOnValidThread());
    broadcaster_.AddOrUpdateSink(sink, wants);
  }

  void Stop() {
    RTC_DCHECK(task_queue_);
    task_queue_.reset();
  }

 private:
  class FrameTask : public rtc::QueuedTask {
   public:
    FrameTask(Config config, rtc::VideoBroadcaster* broadcaster)
        : frame_interval_ms_(config.frame_interval_ms),
          frame_source_(
              config.width,
              config.height,
              config.frame_interval_ms * rtc::kNumMicrosecsPerMillisec,
              config.timestamp_offset_ms * rtc::kNumMicrosecsPerMillisec),
          broadcaster_(broadcaster) {
      frame_source_.SetRotation(config.rotation);
    }

    bool Run() override {
      if (broadcaster_->wants().rotation_applied) {
        broadcaster_->OnFrame(frame_source_.GetFrameRotationApplied());
      } else {
        broadcaster_->OnFrame(frame_source_.GetFrame());
      }

      rtc::TaskQueue::Current()->PostDelayedTask(absl::WrapUnique(this),
                                                 frame_interval_ms_);
      return false;
    }
    int frame_interval_ms_;
    cricket::FakeFrameSource frame_source_;
    rtc::VideoBroadcaster* broadcaster_;
  };

  rtc::ThreadChecker thread_checker_;

  rtc::VideoBroadcaster broadcaster_;

  std::unique_ptr<rtc::TaskQueue> task_queue_;
};

}  // namespace webrtc

#endif  // PC_TEST_FAKEPERIODICVIDEOSOURCE_H_
