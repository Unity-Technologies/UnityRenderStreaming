// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_GAMEPAD_GAMEPAD_SHARED_MEMORY_READER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_GAMEPAD_GAMEPAD_SHARED_MEMORY_READER_H_

#include <memory>

#include "device/base/synchronization/shared_memory_seqlock_buffer.h"
#include "device/gamepad/public/cpp/gamepads.h"
#include "device/gamepad/public/mojom/gamepad.mojom-blink.h"
#include "device/gamepad/public/mojom/gamepad_hardware_buffer.h"
#include "mojo/public/cpp/bindings/binding.h"
#include "mojo/public/cpp/system/buffer.h"
#include "third_party/blink/public/platform/web_gamepad_listener.h"
#include "third_party/blink/renderer/platform/wtf/noncopyable.h"

namespace blink {

class LocalFrame;

class GamepadSharedMemoryReader : public device::mojom::blink::GamepadObserver {
  WTF_MAKE_NONCOPYABLE(GamepadSharedMemoryReader);

 public:
  explicit GamepadSharedMemoryReader(LocalFrame& frame);
  ~GamepadSharedMemoryReader() override;

  void SampleGamepads(device::Gamepads& gamepads);
  void Start(blink::WebGamepadListener* listener);
  void Stop();

 protected:
  void SendStartMessage();
  void SendStopMessage();

 private:
  // device::mojom::blink::GamepadObserver methods.
  void GamepadConnected(uint32_t index,
                        const device::Gamepad& gamepad) override;
  void GamepadDisconnected(uint32_t index,
                           const device::Gamepad& gamepad) override;
  void GamepadButtonOrAxisChanged(uint32_t index,
                                  const device::Gamepad& gamepad) override;

  base::ReadOnlySharedMemoryRegion renderer_shared_buffer_region_;
  base::ReadOnlySharedMemoryMapping renderer_shared_buffer_mapping_;
  const device::GamepadHardwareBuffer* gamepad_hardware_buffer_ = nullptr;

  bool ever_interacted_with_ = false;

  mojo::Binding<device::mojom::blink::GamepadObserver> binding_;
  device::mojom::blink::GamepadMonitorPtr gamepad_monitor_;
  blink::WebGamepadListener* listener_ = nullptr;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_GAMEPAD_GAMEPAD_SHARED_MEMORY_READER_H_
