// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_AUDIO_PARAM_MAP_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_AUDIO_PARAM_MAP_H_

#include "third_party/blink/renderer/bindings/core/v8/maplike.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_binding_for_core.h"
#include "third_party/blink/renderer/modules/webaudio/audio_param.h"
#include "third_party/blink/renderer/platform/bindings/exception_state.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class AudioParam;

class AudioParamMap final : public ScriptWrappable,
                            public Maplike<String, AudioParam*> {
  DEFINE_WRAPPERTYPEINFO();

 public:
  explicit AudioParamMap(
      const HeapHashMap<String, Member<AudioParam>>& parameter_map);

  // IDL attributes / methods
  size_t size() const { return parameter_map_.size(); }

  AudioParam* At(String name) { return parameter_map_.at(name); }
  bool Contains(String name) { return parameter_map_.Contains(name); }

  void Trace(blink::Visitor* visitor) override {
    visitor->Trace(parameter_map_);
    ScriptWrappable::Trace(visitor);
  }

 private:
  PairIterable<String, AudioParam*>::IterationSource* StartIteration(
      ScriptState*,
      ExceptionState&) override;
  bool GetMapEntry(ScriptState*,
                   const String& key,
                   AudioParam*&,
                   ExceptionState&) override;

  const HeapHashMap<String, Member<AudioParam>> parameter_map_;
};

}  // namespace blink

#endif
