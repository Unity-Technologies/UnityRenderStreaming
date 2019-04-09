// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_ANIMATION_COMPOSITOR_ANIMATION_CLIENT_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_ANIMATION_COMPOSITOR_ANIMATION_CLIENT_H_

#include "third_party/blink/public/platform/web_common.h"

namespace blink {

class CompositorAnimation;

// A client for compositor representation of Animation.
class BLINK_PLATFORM_EXPORT CompositorAnimationClient {
 public:
  virtual ~CompositorAnimationClient();

  virtual CompositorAnimation* GetCompositorAnimation() const = 0;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_ANIMATION_COMPOSITOR_ANIMATION_CLIENT_H_
