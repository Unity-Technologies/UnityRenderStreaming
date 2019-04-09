// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_TRANSFORM_FEEDBACK_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_TRANSFORM_FEEDBACK_H_

#include "base/memory/scoped_refptr.h"
#include "third_party/blink/renderer/modules/webgl/webgl_context_object.h"
#include "third_party/blink/renderer/modules/webgl/webgl_program.h"

namespace blink {

class WebGL2RenderingContextBase;
class WebGLBuffer;

class WebGLTransformFeedback : public WebGLContextObject {
  DEFINE_WRAPPERTYPEINFO();

 public:
  enum TFType {
    TFTypeDefault,
    TFTypeUser,
  };

  explicit WebGLTransformFeedback(WebGL2RenderingContextBase*, TFType);
  ~WebGLTransformFeedback() override;

  GLuint Object() const { return object_; }

  static WebGLTransformFeedback* Create(WebGL2RenderingContextBase*, TFType);

  bool IsDefaultObject() const { return type_ == TFTypeDefault; }

  GLenum GetTarget() const { return target_; }
  void SetTarget(GLenum);

  bool HasEverBeenBound() const { return object_ && target_; }

  WebGLProgram* GetProgram() const { return program_; }
  void SetProgram(WebGLProgram*);

  // These are the indexed bind points for transform feedback buffers.
  // Returns false if index is out of range and the caller should
  // synthesize a GL error.
  bool SetBoundIndexedTransformFeedbackBuffer(GLuint index, WebGLBuffer*);
  bool GetBoundIndexedTransformFeedbackBuffer(GLuint index,
                                              WebGLBuffer** outBuffer) const;
  bool HasEnoughBuffers(GLuint num_required) const;

  bool UsesBuffer(WebGLBuffer*);
  void UnbindBuffer(WebGLBuffer*);

  void Trace(blink::Visitor*) override;

  bool active() const { return active_; }
  bool paused() const { return paused_; }
  const HeapVector<TraceWrapperMember<WebGLBuffer>>&
  bound_indexed_transform_feedback_buffers() const {
    return bound_indexed_transform_feedback_buffers_;
  }

  void SetActive(bool active) {
    active_ = active;
    DCHECK(active_ || !paused_);
  }
  void SetPaused(bool paused) {
    paused_ = paused;
    DCHECK(active_ || !paused_);
  }

  bool ValidateProgramForResume(WebGLProgram*) const;

 private:
  void DispatchDetached(gpu::gles2::GLES2Interface*);
  bool HasObject() const override { return object_ != 0; }
  void DeleteObjectImpl(gpu::gles2::GLES2Interface*) override;

  GLuint object_;

  TFType type_;
  GLenum target_;

  HeapVector<TraceWrapperMember<WebGLBuffer>>
      bound_indexed_transform_feedback_buffers_;

  Member<WebGLProgram> program_;
  unsigned program_link_count_;
  bool active_;
  bool paused_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_TRANSFORM_FEEDBACK_H_
