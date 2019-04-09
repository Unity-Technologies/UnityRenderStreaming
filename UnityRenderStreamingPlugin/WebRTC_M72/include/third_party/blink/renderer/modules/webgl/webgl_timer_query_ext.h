// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_TIMER_QUERY_EXT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_TIMER_QUERY_EXT_H_

#include "base/single_thread_task_runner.h"
#include "third_party/blink/renderer/modules/webgl/webgl_context_object.h"
#include "third_party/blink/renderer/platform/web_task_runner.h"

namespace gpu {
namespace gles2 {
class GLES2Interface;
}
}

namespace blink {

class WebGLTimerQueryEXT : public WebGLContextObject {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static WebGLTimerQueryEXT* Create(WebGLRenderingContextBase*);

  WebGLTimerQueryEXT(WebGLRenderingContextBase*);
  ~WebGLTimerQueryEXT() override;

  void SetTarget(GLenum target) { target_ = target; }

  GLuint Object() const { return query_id_; }
  bool HasTarget() const { return target_ != 0; }
  GLenum Target() const { return target_; }

  void ResetCachedResult();
  void UpdateCachedResult(gpu::gles2::GLES2Interface*);

  bool IsQueryResultAvailable();
  GLuint64 GetQueryResult();

 private:
  bool HasObject() const override { return query_id_ != 0; }
  void DeleteObjectImpl(gpu::gles2::GLES2Interface*) override;

  void ScheduleAllowAvailabilityUpdate();
  void AllowAvailabilityUpdate();

  GLenum target_;
  GLuint query_id_;

  bool can_update_availability_;
  bool query_result_available_;
  GLuint64 query_result_;

  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  TaskHandle task_handle_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_TIMER_QUERY_EXT_H_
