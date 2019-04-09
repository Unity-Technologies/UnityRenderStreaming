// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGPU_WEBGPU_ADAPTER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGPU_WEBGPU_ADAPTER_H_

#include "third_party/blink/renderer/core/execution_context/execution_context.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class WebGPUDevice;

class WebGPUAdapter final : public ScriptWrappable {
  DISALLOW_COPY_AND_ASSIGN(WebGPUAdapter);
  DEFINE_WRAPPERTYPEINFO();

 public:
  static WebGPUAdapter* Create(const String& name);

  WebGPUAdapter(const String& name);

  const String& name() const;

  WebGPUDevice* createDevice(ExecutionContext*);

 private:
  String name_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGPU_WEBGPU_ADAPTER_H_
