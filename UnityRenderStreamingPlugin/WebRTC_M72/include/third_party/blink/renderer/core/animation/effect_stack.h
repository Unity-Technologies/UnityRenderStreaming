/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_EFFECT_STACK_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_EFFECT_STACK_H_

#include "base/macros.h"
#include "third_party/blink/renderer/core/animation/animation.h"
#include "third_party/blink/renderer/core/animation/effect_model.h"
#include "third_party/blink/renderer/core/animation/keyframe_effect.h"
#include "third_party/blink/renderer/core/animation/property_handle.h"
#include "third_party/blink/renderer/core/animation/sampled_effect.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/geometry/float_box.h"
#include "third_party/blink/renderer/platform/wtf/hash_set.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class InertEffect;

// Represents the order in which a sequence of SampledEffects should apply.
// This sequence is broken down to per PropertyHandle granularity.
class CORE_EXPORT EffectStack {
  DISALLOW_NEW();

 public:
  EffectStack();

  void Add(SampledEffect* sampled_effect) {
    sampled_effects_.push_back(sampled_effect);
  }
  bool IsEmpty() const { return sampled_effects_.IsEmpty(); }
  bool HasActiveAnimationsOnCompositor(const PropertyHandle&) const;

  using PropertyHandleFilter = bool (*)(const PropertyHandle&);
  bool AffectsProperties(PropertyHandleFilter) const;
  static ActiveInterpolationsMap ActiveInterpolations(
      EffectStack*,
      const HeapVector<Member<const InertEffect>>* new_animations,
      const HeapHashSet<Member<const Animation>>* suppressed_animations,
      KeyframeEffect::Priority,
      PropertyHandleFilter = nullptr);

  void Trace(blink::Visitor*);

 private:
  void RemoveRedundantSampledEffects();

  // Effects sorted by priority. Lower priority at the start of the list.
  HeapVector<Member<SampledEffect>> sampled_effects_;

  friend class AnimationEffectStackTest;
  DISALLOW_COPY_AND_ASSIGN(EffectStack);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_EFFECT_STACK_H_
