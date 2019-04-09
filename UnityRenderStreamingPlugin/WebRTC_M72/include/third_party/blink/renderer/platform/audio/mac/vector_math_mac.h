// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_AUDIO_MAC_VECTOR_MATH_MAC_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_AUDIO_MAC_VECTOR_MATH_MAC_H_

#include <Accelerate/Accelerate.h>

#include "build/build_config.h"
#include "third_party/blink/renderer/platform/audio/audio_array.h"

namespace blink {
namespace vector_math {
namespace mac {

// On the Mac we use the highly optimized versions in Accelerate.framework
// In 32-bit mode (__ppc__ or __i386__) <Accelerate/Accelerate.h> includes
// <vecLib/vDSP_translate.h> which defines macros of the same name as
// our namespaced function names, so we must handle this case differently. Other
// architectures (64bit, ARM, etc.) do not include this header file.

static ALWAYS_INLINE void Conv(const float* source_p,
                               int source_stride,
                               const float* filter_p,
                               int filter_stride,
                               float* dest_p,
                               int dest_stride,
                               size_t frames_to_process,
                               size_t filter_size,
                               const AudioFloatArray* /*prepared_filter*/) {
#if defined(ARCH_CPU_X86)
  ::conv(source_p, source_stride, filter_p, filter_stride, dest_p, dest_stride,
         frames_to_process, filter_size);
#else
  vDSP_conv(source_p, source_stride, filter_p, filter_stride, dest_p,
            dest_stride, frames_to_process, filter_size);
#endif
}

static ALWAYS_INLINE void Vadd(const float* source1p,
                               int source_stride1,
                               const float* source2p,
                               int source_stride2,
                               float* dest_p,
                               int dest_stride,
                               size_t frames_to_process) {
#if defined(ARCH_CPU_X86)
  ::vadd(source1p, source_stride1, source2p, source_stride2, dest_p,
         dest_stride, frames_to_process);
#else
  vDSP_vadd(source1p, source_stride1, source2p, source_stride2, dest_p,
            dest_stride, frames_to_process);
#endif
}

static ALWAYS_INLINE void Vclip(const float* source_p,
                                int source_stride,
                                const float* low_threshold_p,
                                const float* high_threshold_p,
                                float* dest_p,
                                int dest_stride,
                                size_t frames_to_process) {
  vDSP_vclip(source_p, source_stride, low_threshold_p, high_threshold_p, dest_p,
             dest_stride, frames_to_process);
}

static ALWAYS_INLINE void Vmaxmgv(const float* source_p,
                                  int source_stride,
                                  float* max_p,
                                  size_t frames_to_process) {
  vDSP_maxmgv(source_p, source_stride, max_p, frames_to_process);
}

static ALWAYS_INLINE void Vmul(const float* source1p,
                               int source_stride1,
                               const float* source2p,
                               int source_stride2,
                               float* dest_p,
                               int dest_stride,
                               size_t frames_to_process) {
#if defined(ARCH_CPU_X86)
  ::vmul(source1p, source_stride1, source2p, source_stride2, dest_p,
         dest_stride, frames_to_process);
#else
  vDSP_vmul(source1p, source_stride1, source2p, source_stride2, dest_p,
            dest_stride, frames_to_process);
#endif
}

static ALWAYS_INLINE void Vsma(const float* source_p,
                               int source_stride,
                               const float* scale,
                               float* dest_p,
                               int dest_stride,
                               size_t frames_to_process) {
  vDSP_vsma(source_p, source_stride, scale, dest_p, dest_stride, dest_p,
            dest_stride, frames_to_process);
}

static ALWAYS_INLINE void Vsmul(const float* source_p,
                                int source_stride,
                                const float* scale,
                                float* dest_p,
                                int dest_stride,
                                size_t frames_to_process) {
#if defined(ARCH_CPU_X86)
  ::vsmul(source_p, source_stride, scale, dest_p, dest_stride,
          frames_to_process);
#else
  vDSP_vsmul(source_p, source_stride, scale, dest_p, dest_stride,
             frames_to_process);
#endif
}

static ALWAYS_INLINE void Vsvesq(const float* source_p,
                                 int source_stride,
                                 float* sum_p,
                                 size_t frames_to_process) {
  vDSP_svesq(source_p, source_stride, sum_p, frames_to_process);
}

static ALWAYS_INLINE void Zvmul(const float* real1p,
                                const float* imag1p,
                                const float* real2p,
                                const float* imag2p,
                                float* real_dest_p,
                                float* imag_dest_p,
                                size_t frames_to_process) {
  DSPSplitComplex sc1;
  DSPSplitComplex sc2;
  DSPSplitComplex dest;
  sc1.realp = const_cast<float*>(real1p);
  sc1.imagp = const_cast<float*>(imag1p);
  sc2.realp = const_cast<float*>(real2p);
  sc2.imagp = const_cast<float*>(imag2p);
  dest.realp = real_dest_p;
  dest.imagp = imag_dest_p;
#if defined(ARCH_CPU_X86)
  ::zvmul(&sc1, 1, &sc2, 1, &dest, 1, frames_to_process, 1);
#else
  vDSP_zvmul(&sc1, 1, &sc2, 1, &dest, 1, frames_to_process, 1);
#endif
}

}  // namespace mac
}  // namespace vector_math
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_AUDIO_MAC_VECTOR_MATH_MAC_H_
