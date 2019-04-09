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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_INERT_EFFECT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_INERT_EFFECT_H_

#include "third_party/blink/renderer/core/animation/animation_effect.h"
#include "third_party/blink/renderer/core/animation/keyframe_effect_model.h"
#include "third_party/blink/renderer/core/core_export.h"

namespace blink {

// Lightweight subset of KeyframeEffect.
// Used to transport data for deferred KeyframeEffect construction and one off
// Interpolation sampling.
class CORE_EXPORT InertEffect final : public AnimationEffect {
 public:
  static InertEffect* Create(KeyframeEffectModelBase*,
                             const Timing&,
                             bool paused,
                             double inherited_time);

  InertEffect(KeyframeEffectModelBase*,
              const Timing&,
              bool paused,
              double inherited_time);

  void Sample(HeapVector<Member<Interpolation>>&) const;
  KeyframeEffectModelBase* Model() const { return model_.Get(); }
  bool Paused() const { return paused_; }

  bool IsInertEffect() const final { return true; }

  void Trace(blink::Visitor*) override;

 protected:
  void UpdateChildrenAndEffects() const override {}
  double CalculateTimeToEffectChange(
      bool forwards,
      double inherited_time,
      double time_to_next_iteration) const override;

 private:
  Member<KeyframeEffectModelBase> model_;
  bool paused_;
  double inherited_time_;
};

DEFINE_TYPE_CASTS(InertEffect,
                  AnimationEffect,
                  animationEffect,
                  animationEffect->IsInertEffect(),
                  animationEffect.IsInertEffect());

}  // namespace blink

#endif
