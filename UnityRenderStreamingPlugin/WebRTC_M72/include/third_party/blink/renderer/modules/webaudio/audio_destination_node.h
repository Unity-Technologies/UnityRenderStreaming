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

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_AUDIO_DESTINATION_NODE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_AUDIO_DESTINATION_NODE_H_

#include "third_party/blink/renderer/modules/webaudio/audio_node.h"
#include "third_party/blink/renderer/platform/wtf/atomics.h"

namespace blink {

// The AudioDestinationHandler (ADH) is a base class for the rendering backend
// for AudioDestinatioNode. It contains common information required for the
// rendering such as current sample frame, sample rate and maximum channel count
// of the context.
class AudioDestinationHandler : public AudioHandler {
 public:
  AudioDestinationHandler(AudioNode&);
  ~AudioDestinationHandler() override;

  // The method MUST NOT be invoked when rendering a graph because the
  // destination node is a sink. Instead, this node gets pulled by the
  // underlying renderer (audio hardware or worker thread).
  void Process(size_t) final { NOTREACHED(); }

  virtual void StartRendering() = 0;
  virtual void StopRendering() = 0;

  // The render thread needs to be changed after Worklet JS code is loaded by
  // AudioWorklet. This method ensures the switching of render thread and the
  // restart of the context.
  virtual void RestartRendering() = 0;

  size_t CurrentSampleFrame() const {
    return AcquireLoad(&current_sample_frame_);
  }

  double CurrentTime() const {
    return CurrentSampleFrame() / SampleRate();
  }

  virtual double SampleRate() const = 0;
  virtual unsigned long MaxChannelCount() const = 0;

  void ContextDestroyed() { is_execution_context_destroyed_ = true; }
  bool IsExecutionContextDestroyed() const {
    return is_execution_context_destroyed_;
  }

 protected:
  // The number of sample frames processed by the destination so far.
  size_t current_sample_frame_;

 private:
  // True if the execution context is being destroyed.  If this is true, the
  // destination ndoe must avoid checking for or accessing the execution
  // context.
  bool is_execution_context_destroyed_ = false;
};

// -----------------------------------------------------------------------------

// AudioDestinationNode (ADN) is a base class of two different types of nodes:
//   1. DefaultDestinationNode for AudioContext (real time)
//   2. OfflineDestinationNode for OfflineAudioContext (non-real time)
// They have different rendering mechanisms, so the AudioDestinationHandler
// (ADH), which is a counterpart of the destination node, encapsulates a
// different rendering backend.
class AudioDestinationNode : public AudioNode {
  DEFINE_WRAPPERTYPEINFO();

 public:
  unsigned long maxChannelCount() const;

  // Returns its own handler object instead of a generic one from
  // AudioNode::Handler().
  AudioDestinationHandler& GetAudioDestinationHandler() const;

 protected:
  AudioDestinationNode(BaseAudioContext&);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_AUDIO_DESTINATION_NODE_H_
