// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_LOGICAL_VALUES_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_LOGICAL_VALUES_H_

#include "third_party/blink/renderer/core/style/computed_style.h"

namespace blink {

// The following functions resolve CSS logical values and return other ones
// as-is. They can be called with the value to be resolved and the TextDirection
// or WritingMode that will be used for that resolution. Alternatively, they
// also accept two ComputedStyle arguments, the first one to retrieve the value,
// and the second one to get the TextDirection or WritingMode.

EClear ResolvedClear(EClear, TextDirection);
inline EClear ResolvedClear(const ComputedStyle& style,
                            const ComputedStyle& cb_style) {
  return ResolvedClear(style.Clear(), cb_style.Direction());
}

EFloat ResolvedFloating(EFloat, TextDirection);
inline EFloat ResolvedFloating(const ComputedStyle& style,
                               const ComputedStyle& cb_style) {
  return ResolvedFloating(style.Floating(), cb_style.Direction());
}

EResize ResolvedResize(EResize, WritingMode);
inline EResize ResolvedResize(const ComputedStyle& style,
                              const ComputedStyle& cb_style) {
  return ResolvedResize(style.Resize(), cb_style.GetWritingMode());
}

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_LOGICAL_VALUES_H_
