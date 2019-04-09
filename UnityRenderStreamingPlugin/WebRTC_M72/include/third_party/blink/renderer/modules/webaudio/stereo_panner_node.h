// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_STEREO_PANNER_NODE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_STEREO_PANNER_NODE_H_

#include <memory>
#include "base/gtest_prod_util.h"
#include "third_party/blink/renderer/modules/webaudio/audio_node.h"
#include "third_party/blink/renderer/modules/webaudio/audio_param.h"
#include "third_party/blink/renderer/platform/audio/audio_bus.h"
#include "third_party/blink/renderer/platform/audio/stereo_panner.h"

namespace blink {

class BaseAudioContext;
class StereoPannerOptions;

// StereoPannerNode is an AudioNode with one input and one output. It is
// specifically designed for equal-power stereo panning.
class StereoPannerHandler final : public AudioHandler {
 public:
  static scoped_refptr<StereoPannerHandler> Create(AudioNode&,
                                                   float sample_rate,
                                                   AudioParamHandler& pan);
  ~StereoPannerHandler() override;

  void Process(size_t frames_to_process) override;
  void ProcessOnlyAudioParams(size_t frames_to_process) override;
  void Initialize() override;

  void SetChannelCount(unsigned long, ExceptionState&) final;
  void SetChannelCountMode(const String&, ExceptionState&) final;

  double TailTime() const override { return 0; }
  double LatencyTime() const override { return 0; }
  bool RequiresTailProcessing() const final { return false; }

 private:
  StereoPannerHandler(AudioNode&, float sample_rate, AudioParamHandler& pan);

  std::unique_ptr<StereoPanner> stereo_panner_;
  scoped_refptr<AudioParamHandler> pan_;

  AudioFloatArray sample_accurate_pan_values_;

  FRIEND_TEST_ALL_PREFIXES(StereoPannerNodeTest, StereoPannerLifetime);
};

class StereoPannerNode final : public AudioNode {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static StereoPannerNode* Create(BaseAudioContext&, ExceptionState&);
  static StereoPannerNode* Create(BaseAudioContext*,
                                  const StereoPannerOptions*,
                                  ExceptionState&);
  void Trace(blink::Visitor*) override;

  AudioParam* pan() const;

 private:
  StereoPannerNode(BaseAudioContext&);

  Member<AudioParam> pan_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_STEREO_PANNER_NODE_H_
