/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_AUDIO_HRTF_ELEVATION_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_AUDIO_HRTF_ELEVATION_H_

#include <memory>
#include "base/memory/scoped_refptr.h"
#include "third_party/blink/renderer/platform/audio/hrtf_kernel.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/noncopyable.h"
#include "third_party/blink/renderer/platform/wtf/text/cstring.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

// HRTFElevation contains all of the HRTFKernels (one left ear and one right ear
// per azimuth angle) for a particular elevation.

class PLATFORM_EXPORT HRTFElevation {
  USING_FAST_MALLOC(HRTFElevation);
  WTF_MAKE_NONCOPYABLE(HRTFElevation);

 public:
  // Loads and returns an HRTFElevation with the given HRTF database subject
  // name and elevation from browser (or WebKit.framework) resources.
  // Normally, there will only be a single HRTF database set, but this API
  // supports the possibility of multiple ones with different names.
  // Interpolated azimuths will be generated based on InterpolationFactor.
  // Valid values for elevation are -45 -> +90 in 15 degree increments.
  static std::unique_ptr<HRTFElevation> CreateForSubject(
      const String& subject_name,
      int elevation,
      float sample_rate);

  // Given two HRTFElevations, and an interpolation factor x: 0 -> 1, returns an
  // interpolated HRTFElevation.
  static std::unique_ptr<HRTFElevation> CreateByInterpolatingSlices(
      HRTFElevation* hrtf_elevation1,
      HRTFElevation* hrtf_elevation2,
      float x,
      float sample_rate);

  // Returns the list of left or right ear HRTFKernels for all the azimuths
  // going from 0 to 360 degrees.
  HRTFKernelList* KernelListL() { return kernel_list_l_.get(); }
  HRTFKernelList* KernelListR() { return kernel_list_r_.get(); }

  double ElevationAngle() const { return elevation_angle_; }
  unsigned NumberOfAzimuths() const { return kNumberOfTotalAzimuths; }
  float SampleRate() const { return sample_rate_; }

  // Returns the left and right kernels for the given azimuth index.
  // The interpolated delays based on azimuthBlend: 0 -> 1 are returned in
  // frameDelayL and frameDelayR.
  void GetKernelsFromAzimuth(double azimuth_blend,
                             unsigned azimuth_index,
                             HRTFKernel*& kernel_l,
                             HRTFKernel*& kernel_r,
                             double& frame_delay_l,
                             double& frame_delay_r);

  // Spacing, in degrees, between every azimuth loaded from resource.
  static const unsigned kAzimuthSpacing;

  // Number of azimuths loaded from resource.
  static const unsigned kNumberOfRawAzimuths;

  // Interpolates by this factor to get the total number of azimuths from every
  // azimuth loaded from resource.
  static const unsigned kInterpolationFactor;

  // Total number of azimuths after interpolation.
  static const unsigned kNumberOfTotalAzimuths;

  // Given a specific azimuth and elevation angle, returns the left and right
  // HRTFKernel.
  // Valid values for azimuth are 0 -> 345 in 15 degree increments.
  // Valid values for elevation are -45 -> +90 in 15 degree increments.
  // Returns true on success.
  static bool CalculateKernelsForAzimuthElevation(
      int azimuth,
      int elevation,
      float sample_rate,
      const String& subject_name,
      std::unique_ptr<HRTFKernel>& kernel_l,
      std::unique_ptr<HRTFKernel>& kernel_r);

 private:
  HRTFElevation(std::unique_ptr<HRTFKernelList> kernel_list_l,
                std::unique_ptr<HRTFKernelList> kernel_list_r,
                int elevation,
                float sample_rate)
      : kernel_list_l_(std::move(kernel_list_l)),
        kernel_list_r_(std::move(kernel_list_r)),
        elevation_angle_(elevation),
        sample_rate_(sample_rate) {}

  std::unique_ptr<HRTFKernelList> kernel_list_l_;
  std::unique_ptr<HRTFKernelList> kernel_list_r_;
  double elevation_angle_;
  float sample_rate_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_AUDIO_HRTF_ELEVATION_H_
