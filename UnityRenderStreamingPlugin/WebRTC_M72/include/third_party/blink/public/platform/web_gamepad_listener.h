// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_GAMEPAD_LISTENER_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_GAMEPAD_LISTENER_H_

#include "third_party/blink/public/platform/web_platform_event_listener.h"

namespace device {
class Gamepad;
}

namespace blink {

class WebGamepadListener : public WebPlatformEventListener {
 public:
  // Called when a gamepad is connected. |index| is the index of the gamepad in
  // the gamepad array, and |gamepad| is a reference to the connected gamepad.
  virtual void DidConnectGamepad(uint32_t index,
                                 const device::Gamepad& gamepad) = 0;

  // Called when a gamepad is disconnected. |index| is the former index of the
  // gamepad in the gamepad array, and |gamepad| is a reference to the
  // connected gamepad.
  virtual void DidDisconnectGamepad(uint32_t index,
                                    const device::Gamepad& gamepad) = 0;

  // Called when a button or axis is changed on a connected gamepad. |index| is
  // the index of the gamepad in the gamepad array, and |gamepad| is a reference
  // to the gamepad.
  virtual void ButtonOrAxisDidChange(uint32_t index,
                                     const device::Gamepad& gamepad) = 0;

 protected:
  ~WebGamepadListener() override = default;
};

}  // namespace blink

#endif
