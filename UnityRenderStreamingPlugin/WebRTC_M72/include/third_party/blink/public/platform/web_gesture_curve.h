/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_GESTURE_CURVE_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_GESTURE_CURVE_H_

#include "third_party/blink/public/platform/web_float_size.h"
#include "ui/gfx/geometry/vector2d_f.h"

namespace blink {

// Abstract interface for curves used by ActivePlatformGestureAnimation. A
// WebGestureCurve defines the animation parameters as a function of time
// (zero-based), and applies the parameters directly to the target of the
// animation.
class WebGestureCurve {
 public:
  virtual ~WebGestureCurve() = default;

  // Returns false if curve has finished and can no longer advance.
  // This function is used for browser side fling.
  // TODO(dcheng): This parameter should be a base::TimeDelta, but there is
  // incorrect usage of base::TimeTicks throughout the gesture code. Fix in a
  // followup.
  virtual bool Advance(double time,
                       gfx::Vector2dF& out_current_velocity,
                       gfx::Vector2dF& out_delta_to_scroll) = 0;
};

}  // namespace blink

#endif
