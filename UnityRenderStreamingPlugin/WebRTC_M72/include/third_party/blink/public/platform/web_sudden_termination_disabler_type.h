// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_SUDDEN_TERMINATION_DISABLER_TYPE_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_SUDDEN_TERMINATION_DISABLER_TYPE_H_

namespace blink {

// Used when elements preventing the sudden termination of the frame become
// present or become absent.
using WebSuddenTerminationDisablerType = uint8_t;
const WebSuddenTerminationDisablerType kBeforeUnloadHandler = 1 << 0;
const WebSuddenTerminationDisablerType kUnloadHandler = 1 << 1;

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_SUDDEN_TERMINATION_DISABLER_TYPE_H_
