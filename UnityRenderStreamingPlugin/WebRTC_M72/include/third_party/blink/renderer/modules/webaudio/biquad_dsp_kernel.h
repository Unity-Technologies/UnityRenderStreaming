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

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_BIQUAD_DSP_KERNEL_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_BIQUAD_DSP_KERNEL_H_

#include "third_party/blink/renderer/modules/webaudio/biquad_processor.h"
#include "third_party/blink/renderer/platform/audio/audio_dsp_kernel.h"
#include "third_party/blink/renderer/platform/audio/biquad.h"

namespace blink {

class BiquadProcessor;

// BiquadDSPKernel is an AudioDSPKernel and is responsible for filtering one
// channel of a BiquadProcessor using a Biquad object.

class BiquadDSPKernel final : public AudioDSPKernel {
 public:
  explicit BiquadDSPKernel(BiquadProcessor* processor)
      : AudioDSPKernel(processor),
        tail_time_(std::numeric_limits<double>::infinity()) {}

  // AudioDSPKernel
  void Process(const float* source,
               float* dest,
               size_t frames_to_process) override;
  void Reset() override { biquad_.Reset(); }

  // Get the magnitude and phase response of the filter at the given
  // set of frequencies (in Hz). The phase response is in radians.
  void GetFrequencyResponse(int n_frequencies,
                            const float* frequency_hz,
                            float* mag_response,
                            float* phase_response);

  bool RequiresTailProcessing() const final;
  double TailTime() const override;
  double LatencyTime() const override;

 protected:
  Biquad biquad_;
  BiquadProcessor* GetBiquadProcessor() {
    return static_cast<BiquadProcessor*>(Processor());
  }

  // To prevent audio glitches when parameters are changed,
  // dezippering is used to slowly change the parameters.
  void UpdateCoefficientsIfNecessary(int);
  // Update the biquad cofficients with the given parameters
  void UpdateCoefficients(int,
                          const float* frequency,
                          const float* q,
                          const float* gain,
                          const float* detune);

 private:
  // Compute the tail time using the BiquadFilter coefficients at
  // index |coef_index|.
  void UpdateTailTime(int coef_index);

  // Synchronize process() with getting and setting the filter coefficients.
  mutable Mutex process_lock_;

  // The current tail time for biquad filter.
  double tail_time_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_BIQUAD_DSP_KERNEL_H_
