// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_CSS_INTERPOLATION_ENVIRONMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_CSS_INTERPOLATION_ENVIRONMENT_H_

#include "third_party/blink/renderer/core/animation/interpolation_environment.h"
#include "third_party/blink/renderer/core/css/resolver/style_resolver_state.h"

namespace blink {

class ComputedStyle;
class CSSVariableResolver;

class CSSInterpolationEnvironment : public InterpolationEnvironment {
 public:
  explicit CSSInterpolationEnvironment(const InterpolationTypesMap& map,
                                       StyleResolverState& state,
                                       CSSVariableResolver* variable_resolver)
      : InterpolationEnvironment(map),
        state_(&state),
        style_(state.Style()),
        variable_resolver_(variable_resolver) {}

  explicit CSSInterpolationEnvironment(const InterpolationTypesMap& map,
                                       const ComputedStyle& style)
      : InterpolationEnvironment(map), style_(&style) {}

  bool IsCSS() const final { return true; }

  StyleResolverState& GetState() {
    DCHECK(state_);
    return *state_;
  }
  const StyleResolverState& GetState() const {
    DCHECK(state_);
    return *state_;
  }

  const ComputedStyle& Style() const {
    DCHECK(style_);
    return *style_;
  }

  bool HasVariableResolver() const { return variable_resolver_; }

  CSSVariableResolver& VariableResolver() const {
    DCHECK(HasVariableResolver());
    return *variable_resolver_;
  }

 private:
  StyleResolverState* state_ = nullptr;
  const ComputedStyle* style_ = nullptr;
  CSSVariableResolver* variable_resolver_ = nullptr;
};

DEFINE_TYPE_CASTS(CSSInterpolationEnvironment,
                  InterpolationEnvironment,
                  value,
                  value->IsCSS(),
                  value.IsCSS());

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_CSS_INTERPOLATION_ENVIRONMENT_H_
