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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_AUDIO_HRTF_PANNER_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_AUDIO_HRTF_PANNER_H_

#include "third_party/blink/renderer/platform/audio/audio_delay_dsp_kernel.h"
#include "third_party/blink/renderer/platform/audio/fft_convolver.h"
#include "third_party/blink/renderer/platform/audio/hrtf_database_loader.h"
#include "third_party/blink/renderer/platform/audio/panner.h"

namespace blink {

class PLATFORM_EXPORT HRTFPanner final : public Panner {
 public:
  HRTFPanner(float sample_rate, HRTFDatabaseLoader*);
  ~HRTFPanner() override;

  // Panner
  void Pan(double azimuth,
           double elevation,
           const AudioBus* input_bus,
           AudioBus* output_bus,
           size_t frames_to_process,
           AudioBus::ChannelInterpretation) override;
  void PanWithSampleAccurateValues(double* azimuth,
                                   double* elevation,
                                   const AudioBus* input_bus,
                                   AudioBus* output_bus,
                                   size_t frames_to_process,
                                   AudioBus::ChannelInterpretation) override;

  void Reset() override;

  size_t FftSize() const { return FftSizeForSampleRate(sample_rate_); }
  static size_t FftSizeForSampleRate(float sample_rate);

  float SampleRate() const { return sample_rate_; }

  double TailTime() const override;
  double LatencyTime() const override;
  bool RequiresTailProcessing() const override;

 private:
  // Given an azimuth angle in the range -180 -> +180, returns the corresponding
  // azimuth index for the database, and azimuthBlend which is an interpolation
  // value from 0 -> 1.
  int CalculateDesiredAzimuthIndexAndBlend(double azimuth,
                                           double& azimuth_blend);

  scoped_refptr<HRTFDatabaseLoader> database_loader_;

  float sample_rate_;

  // We maintain two sets of convolvers for smooth cross-faded interpolations
  // when then azimuth and elevation are dynamically changing.  When the
  // azimuth and elevation are not changing, we simply process with one
  // of the two sets.  Initially we use CrossfadeSelection1 corresponding to
  // m_convolverL1 and m_convolverR1.  Whenever the azimuth or elevation
  // changes, a crossfade is initiated to transition to the new position. So if
  // we're currently processing with CrossfadeSelection1, then we transition to
  // CrossfadeSelection2 (and vice versa).  If we're in the middle of a
  // transition, then we wait until it is complete before initiating a new
  // transition.

  // Selects either the convolver set (m_convolverL1, m_convolverR1) or
  // (m_convolverL2, m_convolverR2).
  enum CrossfadeSelection { kCrossfadeSelection1, kCrossfadeSelection2 };

  CrossfadeSelection crossfade_selection_;

  // azimuth/elevation for CrossfadeSelection1.
  int azimuth_index1_;
  double elevation1_;

  // azimuth/elevation for CrossfadeSelection2.
  int azimuth_index2_;
  double elevation2_;

  // A crossfade value 0 <= m_crossfadeX <= 1.
  float crossfade_x_;

  // Per-sample-frame crossfade value increment.
  float crossfade_incr_;

  FFTConvolver convolver_l1_;
  FFTConvolver convolver_r1_;
  FFTConvolver convolver_l2_;
  FFTConvolver convolver_r2_;

  AudioDelayDSPKernel delay_line_l_;
  AudioDelayDSPKernel delay_line_r_;

  AudioFloatArray temp_l1_;
  AudioFloatArray temp_r1_;
  AudioFloatArray temp_l2_;
  AudioFloatArray temp_r2_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_AUDIO_HRTF_PANNER_H_
