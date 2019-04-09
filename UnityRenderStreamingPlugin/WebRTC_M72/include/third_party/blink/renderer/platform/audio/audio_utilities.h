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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_AUDIO_AUDIO_UTILITIES_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_AUDIO_AUDIO_UTILITIES_H_

#include <cstddef>
#include "third_party/blink/renderer/platform/platform_export.h"

namespace blink {
namespace audio_utilities {

// Rendering quantum size.  This is how many frames are processed at a time for
// each node in the audio graph.
static const unsigned kRenderQuantumFrames = 128;

// Standard functions for converting to and from decibel values from linear.
PLATFORM_EXPORT float LinearToDecibels(float);
PLATFORM_EXPORT float DecibelsToLinear(float);

// timeConstant is the time it takes a first-order linear time-invariant system
// to reach the value 1 - 1/e (around 63.2%) given a step input response.
// discreteTimeConstantForSampleRate() will return the discrete time-constant
// for the specific sampleRate.
PLATFORM_EXPORT double DiscreteTimeConstantForSampleRate(double time_constant,
                                                         double sample_rate);

// Convert the time to a sample frame at the given sample rate.
PLATFORM_EXPORT size_t TimeToSampleFrame(double time, double sample_rate);

// Check that |sampleRate| is a valid rate for AudioBuffers.
PLATFORM_EXPORT bool IsValidAudioBufferSampleRate(float sample_rate);

// Return max/min sample rate supported by AudioBuffers.
PLATFORM_EXPORT float MinAudioBufferSampleRate();
PLATFORM_EXPORT float MaxAudioBufferSampleRate();

// Check to see if x is a power of two.  If x == 0, returns false.
PLATFORM_EXPORT bool IsPowerOfTwo(size_t x);

}  // namespace audio_utilities
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_AUDIO_AUDIO_UTILITIES_H_
