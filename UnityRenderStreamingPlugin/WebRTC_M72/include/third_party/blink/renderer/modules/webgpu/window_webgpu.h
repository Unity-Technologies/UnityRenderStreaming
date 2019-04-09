// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGPU_WINDOW_WEBGPU_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGPU_WINDOW_WEBGPU_H_

#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/supplementable.h"

namespace blink {

class WebGPU;
class LocalDOMWindow;

class WindowWebGPU final : public GarbageCollected<WindowWebGPU>,
                           public Supplement<LocalDOMWindow> {
  USING_GARBAGE_COLLECTED_MIXIN(WindowWebGPU);

 public:
  static const char kSupplementName[];

  static WindowWebGPU& From(LocalDOMWindow&);
  static WebGPU* webgpu(LocalDOMWindow&);
  WebGPU* webgpu() const;

  void Trace(blink::Visitor*) override;

 private:
  explicit WindowWebGPU(LocalDOMWindow&);

  mutable Member<WebGPU> webgpu_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGPU_WINDOW_WEBGPU_H_
