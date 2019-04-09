/*
 * Copyright (C) 2010, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_REALTIME_ANALYSER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_REALTIME_ANALYSER_H_

#include <memory>
#include "third_party/blink/renderer/core/typed_arrays/dom_typed_array.h"
#include "third_party/blink/renderer/platform/audio/audio_array.h"
#include "third_party/blink/renderer/platform/audio/fft_frame.h"
#include "third_party/blink/renderer/platform/wtf/noncopyable.h"

namespace blink {

class AudioBus;

class RealtimeAnalyser final {
  WTF_MAKE_NONCOPYABLE(RealtimeAnalyser);
  DISALLOW_NEW();

 public:
  RealtimeAnalyser();

  size_t FftSize() const { return fft_size_; }
  bool SetFftSize(size_t);

  unsigned FrequencyBinCount() const { return fft_size_ / 2; }

  void SetMinDecibels(double k) { min_decibels_ = k; }
  double MinDecibels() const { return min_decibels_; }

  void SetMaxDecibels(double k) { max_decibels_ = k; }
  double MaxDecibels() const { return max_decibels_; }

  void SetSmoothingTimeConstant(double k) { smoothing_time_constant_ = k; }
  double SmoothingTimeConstant() const { return smoothing_time_constant_; }

  void GetFloatFrequencyData(DOMFloat32Array*, double);
  void GetByteFrequencyData(DOMUint8Array*, double);
  void GetFloatTimeDomainData(DOMFloat32Array*);
  void GetByteTimeDomainData(DOMUint8Array*);

  // The audio thread writes input data here.
  void WriteInput(AudioBus*, size_t frames_to_process);

  static const double kDefaultSmoothingTimeConstant;
  static const double kDefaultMinDecibels;
  static const double kDefaultMaxDecibels;

  static const unsigned kDefaultFFTSize;
  static const unsigned kMinFFTSize;
  static const unsigned kMaxFFTSize;
  static const unsigned kInputBufferSize;

 private:
  // The audio thread writes the input audio here.
  AudioFloatArray input_buffer_;
  unsigned write_index_;

  unsigned GetWriteIndex() const { return AcquireLoad(&write_index_); }
  void SetWriteIndex(unsigned new_index) {
    ReleaseStore(&write_index_, new_index);
  }

  // Input audio is downmixed to this bus before copying to m_inputBuffer.
  scoped_refptr<AudioBus> down_mix_bus_;

  size_t fft_size_;
  std::unique_ptr<FFTFrame> analysis_frame_;
  void DoFFTAnalysis();

  // Convert the contents of magnitudeBuffer to byte values, saving the result
  // in |destination|.
  void ConvertToByteData(DOMUint8Array* destination);

  // Convert magnidue buffer to dB, saving the result in |destination|
  void ConvertFloatToDb(DOMFloat32Array* destination);

  // doFFTAnalysis() stores the floating-point magnitude analysis data here.
  AudioFloatArray magnitude_buffer_;
  AudioFloatArray& MagnitudeBuffer() { return magnitude_buffer_; }

  // A value between 0 and 1 which averages the previous version of
  // m_magnitudeBuffer with the current analysis magnitude data.
  double smoothing_time_constant_;

  // The range used when converting when using getByteFrequencyData().
  double min_decibels_;
  double max_decibels_;

  // Time at which the FFT was last computed.
  double last_analysis_time_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_REALTIME_ANALYSER_H_
