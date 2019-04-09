// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGPU_WEBGPU_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGPU_WEBGPU_H_

#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"

namespace blink {

class WebGPUAdapter;
class WebGPUAdapterDescriptor;

class WebGPU final : public ScriptWrappable {
  DISALLOW_COPY_AND_ASSIGN(WebGPU);
  DEFINE_WRAPPERTYPEINFO();

 public:
  static WebGPU* Create();

  WebGPU();

  WebGPUAdapter* getAdapter(const WebGPUAdapterDescriptor*);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGPU_WEBGPU_H_
