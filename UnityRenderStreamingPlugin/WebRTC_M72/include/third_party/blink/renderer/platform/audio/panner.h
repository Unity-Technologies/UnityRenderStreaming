/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_AUDIO_PANNER_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_AUDIO_PANNER_H_

#include <memory>
#include "third_party/blink/renderer/platform/audio/audio_bus.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/noncopyable.h"

namespace blink {

class HRTFDatabaseLoader;

// Abstract base class for panning a mono or stereo source.

class PLATFORM_EXPORT Panner {
  USING_FAST_MALLOC(Panner);
  WTF_MAKE_NONCOPYABLE(Panner);

 public:
  // This values are used in histograms and should not be renumbered or deleted.
  enum { kPanningModelEqualPower = 0, kPanningModelHRTF = 1 };

  typedef unsigned PanningModel;

  static std::unique_ptr<Panner> Create(PanningModel,
                                        float sample_rate,
                                        HRTFDatabaseLoader*);

  virtual ~Panner() = default;
  ;

  virtual void Pan(double azimuth,
                   double elevation,
                   const AudioBus* input_bus,
                   AudioBus* output_bus,
                   size_t frames_to_process,
                   AudioBus::ChannelInterpretation) = 0;
  virtual void PanWithSampleAccurateValues(double* azimuth,
                                           double* elevation,
                                           const AudioBus* input_bus,
                                           AudioBus* output_bus,
                                           size_t frames_to_process,
                                           AudioBus::ChannelInterpretation) = 0;

  virtual void Reset() = 0;

  virtual double TailTime() const = 0;
  virtual double LatencyTime() const = 0;
  virtual bool RequiresTailProcessing() const = 0;

 protected:
  Panner(PanningModel model) : panning_model_(model) {}

  PanningModel panning_model_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_AUDIO_PANNER_H_
