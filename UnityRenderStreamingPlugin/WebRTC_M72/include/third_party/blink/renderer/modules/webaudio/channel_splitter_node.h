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

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_CHANNEL_SPLITTER_NODE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_CHANNEL_SPLITTER_NODE_H_

#include "base/memory/scoped_refptr.h"
#include "third_party/blink/renderer/modules/webaudio/audio_node.h"

namespace blink {

class BaseAudioContext;
class ChannelSplitterOptions;

class ChannelSplitterHandler final : public AudioHandler {
 public:
  static scoped_refptr<ChannelSplitterHandler>
  Create(AudioNode&, float sample_rate, unsigned number_of_outputs);

  // AudioHandler
  void Process(size_t frames_to_process) override;
  void SetChannelCount(unsigned long, ExceptionState&) final;
  void SetChannelCountMode(const String&, ExceptionState&) final;
  void SetChannelInterpretation(const String&, ExceptionState&) final;

  double TailTime() const override { return 0; }
  double LatencyTime() const override { return 0; }
  bool RequiresTailProcessing() const final { return false; }

 private:
  ChannelSplitterHandler(AudioNode&,
                         float sample_rate,
                         unsigned number_of_outputs);
};

class ChannelSplitterNode final : public AudioNode {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static ChannelSplitterNode* Create(BaseAudioContext&, ExceptionState&);
  static ChannelSplitterNode* Create(BaseAudioContext&,
                                     unsigned number_of_outputs,
                                     ExceptionState&);
  static ChannelSplitterNode* Create(BaseAudioContext*,
                                     const ChannelSplitterOptions*,
                                     ExceptionState&);

 private:
  ChannelSplitterNode(BaseAudioContext&, unsigned number_of_outputs);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_CHANNEL_SPLITTER_NODE_H_
