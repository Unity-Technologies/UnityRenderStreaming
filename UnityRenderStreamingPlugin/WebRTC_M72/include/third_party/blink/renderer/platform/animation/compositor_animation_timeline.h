// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_ANIMATION_COMPOSITOR_ANIMATION_TIMELINE_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_ANIMATION_COMPOSITOR_ANIMATION_TIMELINE_H_

#include <memory>

#include "base/memory/ptr_util.h"
#include "base/memory/scoped_refptr.h"
#include "cc/animation/animation_timeline.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/noncopyable.h"

namespace blink {

class CompositorAnimationClient;

// A compositor representation for cc::AnimationTimeline.
class PLATFORM_EXPORT CompositorAnimationTimeline {
  WTF_MAKE_NONCOPYABLE(CompositorAnimationTimeline);

 public:
  static std::unique_ptr<CompositorAnimationTimeline> Create() {
    return base::WrapUnique(new CompositorAnimationTimeline());
  }

  ~CompositorAnimationTimeline();

  cc::AnimationTimeline* GetAnimationTimeline() const;

  void AnimationAttached(const CompositorAnimationClient&);
  void AnimationDestroyed(const CompositorAnimationClient&);

 private:
  CompositorAnimationTimeline();

  scoped_refptr<cc::AnimationTimeline> animation_timeline_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_ANIMATION_COMPOSITOR_ANIMATION_TIMELINE_H_
