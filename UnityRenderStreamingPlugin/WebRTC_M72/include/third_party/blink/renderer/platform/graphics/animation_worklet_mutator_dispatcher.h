// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_ANIMATION_WORKLET_MUTATOR_DISPATCHER_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_ANIMATION_WORKLET_MUTATOR_DISPATCHER_H_

#include "third_party/blink/renderer/platform/graphics/animation_worklet_mutators_state.h"
#include "third_party/blink/renderer/platform/platform_export.h"

namespace blink {

class PLATFORM_EXPORT AnimationWorkletMutatorDispatcher {
 public:
  virtual ~AnimationWorkletMutatorDispatcher() = default;

  // Run the animation frame callbacks from all connected AnimationWorklets.
  virtual void Mutate(std::unique_ptr<AnimationWorkletDispatcherInput>) = 0;
  // Returns true if Mutate may do something if called 'now'.
  virtual bool HasMutators() = 0;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_ANIMATION_WORKLET_MUTATOR_DISPATCHER_H_
