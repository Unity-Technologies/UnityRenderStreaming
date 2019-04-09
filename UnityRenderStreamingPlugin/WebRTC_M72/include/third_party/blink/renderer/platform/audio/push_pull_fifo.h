// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_AUDIO_PUSH_PULL_FIFO_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_AUDIO_PUSH_PULL_FIFO_H_

#include "third_party/blink/public/platform/web_common.h"
#include "third_party/blink/renderer/platform/audio/audio_bus.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/functional.h"
#include "third_party/blink/renderer/platform/wtf/threading.h"
#include "third_party/blink/renderer/platform/wtf/threading_primitives.h"

namespace blink {

// A configuration data container for PushPullFIFO unit test.
struct PushPullFIFOStateForTest {
  const size_t fifo_length;
  const unsigned number_of_channels;
  const size_t frames_available;
  const size_t index_read;
  const size_t index_write;
  const unsigned overflow_count;
  const unsigned underflow_count;
};

// PushPullFIFO class is an intermediate audio sample storage between
// Blink-WebAudio and the renderer. The renderer's hardware callback buffer size
// varies on the platform, but the WebAudio always renders 128 frames (render
// quantum, RQ) thus FIFO is needed to handle the general case.
//
// Note that this object is concurrently accessed by two threads; WebAudio
// rendering thread (WebThread) in Blink and the audio device thread
// (AudioDeviceThread) from the media renderer. The push/pull operations touch
// most of variables in the class (index_write_, index_read_, frames_available_,
// and fifo_Bus_) so the thread safety must be handled with care.
//
// TODO(hongchan): add a unit test for multi-thread access.
class BLINK_PLATFORM_EXPORT PushPullFIFO {
  USING_FAST_MALLOC(PushPullFIFO);
  WTF_MAKE_NONCOPYABLE(PushPullFIFO);

 public:
  // Maximum FIFO length. (512 render quanta)
  static const size_t kMaxFIFOLength;

  // |fifo_length| cannot exceed |kMaxFIFOLength|. Otherwise it crashes.
  explicit PushPullFIFO(unsigned number_of_channels, size_t fifo_length);
  ~PushPullFIFO();

  // Pushes the rendered frames by WebAudio engine.
  //  - The |input_bus| length is 128 frames (1 render quantum), fixed.
  //  - In case of overflow (FIFO full while push), the existing frames in FIFO
  //    will be overwritten and |index_read_| will be forcibly moved to
  //    |index_write_| to avoid reading overwritten frames.
  void Push(const AudioBus* input_bus);

  // Pulls |frames_requested| by the audio device thread and returns the actual
  // number of frames to be rendered by the source. (i.e. WebAudio graph)
  //  - If |frames_requested| is bigger than the length of |output_bus|, it
  //    violates SECURITY_CHECK().
  //  - If |frames_requested| is bigger than FIFO length, it violates
  //    SECURITY_CHECK().
  //  - In case of underflow (FIFO empty while pull), the remaining space in the
  //    requested output bus will be filled with silence. Thus it will fulfill
  //    the request from the consumer without causing error, but with a glitch.
  size_t Pull(AudioBus* output_bus, size_t frames_requested);

  size_t length() const { return fifo_length_; }
  unsigned NumberOfChannels() const { return fifo_bus_->NumberOfChannels(); }

  // TODO(hongchan): For single thread unit test only. Consider refactoring.
  AudioBus* GetFIFOBusForTest() const { return fifo_bus_.get(); }

  // For single thread unit test only. Get the current configuration that
  // consists of FIFO length, number of channels, read/write index position and
  // under/overflow count.
  const PushPullFIFOStateForTest GetStateForTest() const;

 private:
  // The size of the FIFO.
  const size_t fifo_length_ = 0;

  // For UMA reporting purpose.
  unsigned pull_count_ = 0;
  unsigned overflow_count_ = 0;
  unsigned underflow_count_ = 0;

  // This lock protects variables below.
  Mutex lock_;
  // The number of frames in the FIFO actually available for pulling.
  size_t frames_available_ = 0;
  size_t index_read_ = 0;
  size_t index_write_ = 0;
  scoped_refptr<AudioBus> fifo_bus_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_AUDIO_PUSH_PULL_FIFO_H_
