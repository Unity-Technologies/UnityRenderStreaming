// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_QUERY_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_QUERY_H_

#include "base/single_thread_task_runner.h"
#include "third_party/blink/renderer/modules/webgl/webgl_shared_platform_3d_object.h"
#include "third_party/blink/renderer/platform/web_task_runner.h"

namespace gpu {
namespace gles2 {
class GLES2Interface;
}
}

namespace blink {

class WebGL2RenderingContextBase;

class WebGLQuery : public WebGLSharedPlatform3DObject {
  DEFINE_WRAPPERTYPEINFO();

 public:
  explicit WebGLQuery(WebGL2RenderingContextBase*);
  ~WebGLQuery() override;

  static WebGLQuery* Create(WebGL2RenderingContextBase*);

  void SetTarget(GLenum);
  bool HasTarget() const { return target_ != 0; }
  GLenum GetTarget() const { return target_; }

  void ResetCachedResult();
  void UpdateCachedResult(gpu::gles2::GLES2Interface*);

  bool IsQueryResultAvailable();
  GLuint GetQueryResult();

 protected:
  void DeleteObjectImpl(gpu::gles2::GLES2Interface*) override;

 private:
  bool IsQuery() const override { return true; }

  void ScheduleAllowAvailabilityUpdate();
  void AllowAvailabilityUpdate();

  GLenum target_;

  bool can_update_availability_;
  bool query_result_available_;
  GLuint query_result_;

  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  TaskHandle task_handle_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_QUERY_H_
