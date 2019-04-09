// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_COMMON_SANDBOX_SUPPORT_SANDBOX_SUPPORT_MAC_H_
#define THIRD_PARTY_BLINK_PUBLIC_COMMON_SANDBOX_SUPPORT_SANDBOX_SUPPORT_MAC_H_

namespace blink {

// Named Mac system colors. Each of these corresponds to a selector on
// NSColor.
enum class MacSystemColorID {
  kAlternateSelectedControl,
  kControlBackground,
  kControlDarkShadow,
  kControlHighlight,
  kControlLightHighlight,
  kControlShadow,
  kControlText,
  kDisabledControlText,
  kHeader,
  kHighlight,
  kKeyboardFocusIndicator,
  kMenuBackground,
  kScrollBar,
  kSecondarySelectedControl,
  kSelectedMenuItemText,
  kSelectedText,
  kSelectedTextBackground,
  kShadow,
  kText,
  kWindowBackground,
  kWindowFrame,
  kWindowFrameText,
  kCount,
};

constexpr size_t kMacSystemColorIDCount =
    static_cast<size_t>(MacSystemColorID::kCount);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_COMMON_SANDBOX_SUPPORT_SANDBOX_SUPPORT_MAC_H_
