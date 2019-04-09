// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_SAMPLER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_SAMPLER_H_

#include "third_party/blink/renderer/modules/webgl/webgl_shared_platform_3d_object.h"

namespace blink {

class WebGL2RenderingContextBase;

class WebGLSampler : public WebGLSharedPlatform3DObject {
  DEFINE_WRAPPERTYPEINFO();

 public:
  explicit WebGLSampler(WebGL2RenderingContextBase*);
  ~WebGLSampler() override;

  static WebGLSampler* Create(WebGL2RenderingContextBase*);

 protected:
  void DeleteObjectImpl(gpu::gles2::GLES2Interface*) override;

 private:
  bool IsSampler() const override { return true; }
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_SAMPLER_H_
