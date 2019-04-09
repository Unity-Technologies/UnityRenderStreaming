// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_CROSS_THREAD_AUDIO_WORKLET_PROCESSOR_INFO_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_CROSS_THREAD_AUDIO_WORKLET_PROCESSOR_INFO_H_

#include "third_party/blink/renderer/modules/webaudio/audio_param_descriptor.h"
#include "third_party/blink/renderer/modules/webaudio/audio_worklet_processor_definition.h"

namespace blink {

// A class for shallow repackage of |AudioParamDescriptor|. This is created only
// when requested when the synchronization between AudioWorkletMessagingProxy
// and AudioWorkletGlobalScope.
class CrossThreadAudioParamInfo {
  DISALLOW_NEW();

 public:
  explicit CrossThreadAudioParamInfo(const AudioParamDescriptor* descriptor)
      : name_(descriptor->name().IsolatedCopy()),
        default_value_(descriptor->defaultValue()),
        max_value_(descriptor->maxValue()),
        min_value_(descriptor->minValue()) {}

  const String& Name() const { return name_; }
  float DefaultValue() const { return default_value_; }
  float MaxValue() const { return max_value_; }
  float MinValue() const { return min_value_; }

 private:
  const String name_;
  const float default_value_;
  const float max_value_;
  const float min_value_;
};

// A class for shallow repackage of |AudioWorkletProcessorDefinition|. This is
// created only when requested when the synchronization between
// AudioWorkletMessagingProxy and AudioWorkletGlobalScope.
class CrossThreadAudioWorkletProcessorInfo {
  DISALLOW_NEW();

 public:
  explicit CrossThreadAudioWorkletProcessorInfo(
      const AudioWorkletProcessorDefinition& definition)
      : name_(definition.GetName().IsolatedCopy()) {
    // To avoid unnecessary reallocations of the vector.
    param_info_list_.ReserveInitialCapacity(
        definition.GetAudioParamDescriptorNames().size());

    for (const String& name : definition.GetAudioParamDescriptorNames()) {
      param_info_list_.emplace_back(
          definition.GetAudioParamDescriptor(name));
    }
  }

  const String& Name() const { return name_; }
  Vector<CrossThreadAudioParamInfo> ParamInfoList() { return param_info_list_; }

 private:
  const String name_;
  Vector<CrossThreadAudioParamInfo> param_info_list_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_CROSS_THREAD_AUDIO_WORKLET_PROCESSOR_INFO_H_
