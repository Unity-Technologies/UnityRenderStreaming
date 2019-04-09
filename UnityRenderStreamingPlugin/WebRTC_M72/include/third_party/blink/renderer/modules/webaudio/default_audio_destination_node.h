/*
 * Copyright (C) 2011, Google Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_DEFAULT_AUDIO_DESTINATION_NODE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_DEFAULT_AUDIO_DESTINATION_NODE_H_

#include <memory>
#include "third_party/blink/public/platform/web_audio_latency_hint.h"
#include "third_party/blink/renderer/modules/webaudio/audio_destination_node.h"
#include "third_party/blink/renderer/platform/audio/audio_destination.h"
#include "third_party/blink/renderer/platform/audio/audio_io_callback.h"

namespace blink {

class BaseAudioContext;
class ExceptionState;
class WebAudioLatencyHint;

class DefaultAudioDestinationHandler final : public AudioDestinationHandler,
                                             public AudioIOCallback {
 public:
  static scoped_refptr<DefaultAudioDestinationHandler> Create(
      AudioNode&,
      const WebAudioLatencyHint&);
  ~DefaultAudioDestinationHandler() override;

  // For AudioHandler.
  void Dispose() override;
  void Initialize() override;
  void Uninitialize() override;
  void SetChannelCount(unsigned long, ExceptionState&) override;
  double LatencyTime() const override { return 0; }
  double TailTime() const override { return 0; }
  bool RequiresTailProcessing() const final { return false; }

  // For AudioDestinationHandler.
  void StartRendering() override;
  void StopRendering() override;
  void RestartRendering() override;
  unsigned long MaxChannelCount() const override;
  double SampleRate() const override;

  // For AudioIOCallback. This is invoked by the platform audio destination to
  // get the next render quantum into |destination_bus| and update
  // |output_position|.
  void Render(AudioBus* destination_bus,
              size_t number_of_frames,
              const AudioIOPosition& output_position) final;

  // Returns a hadrware callback buffer size from audio infra.
  size_t GetCallbackBufferSize() const;

  // Returns a given frames-per-buffer size from audio infra.
  int GetFramesPerBuffer() const;

 private:
  explicit DefaultAudioDestinationHandler(AudioNode&,
                                          const WebAudioLatencyHint&);

  void CreatePlatformDestination();
  void StartPlatformDestination();
  void StopPlatformDestination();

  const WebAudioLatencyHint latency_hint_;

  // Holds the audio device thread that runs the real time audio context.
  scoped_refptr<AudioDestination> platform_destination_;
};

// -----------------------------------------------------------------------------

class DefaultAudioDestinationNode final : public AudioDestinationNode {
 public:
  static DefaultAudioDestinationNode* Create(BaseAudioContext*,
                                             const WebAudioLatencyHint&);

  explicit DefaultAudioDestinationNode(BaseAudioContext&,
                                       const WebAudioLatencyHint&);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_DEFAULT_AUDIO_DESTINATION_NODE_H_
