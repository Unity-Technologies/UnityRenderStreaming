// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_VERTEX_ARRAY_OBJECT_BASE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_VERTEX_ARRAY_OBJECT_BASE_H_

#include "third_party/blink/renderer/modules/webgl/webgl_buffer.h"
#include "third_party/blink/renderer/modules/webgl/webgl_context_object.h"
#include "third_party/blink/renderer/platform/bindings/trace_wrapper_member.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class WebGLVertexArrayObjectBase : public WebGLContextObject {
 public:
  enum VaoType {
    kVaoTypeDefault,
    kVaoTypeUser,
  };

  ~WebGLVertexArrayObjectBase() override;

  GLuint Object() const { return object_; }

  bool IsDefaultObject() const { return type_ == kVaoTypeDefault; }

  bool HasEverBeenBound() const { return Object() && has_ever_been_bound_; }
  void SetHasEverBeenBound() { has_ever_been_bound_ = true; }

  WebGLBuffer* BoundElementArrayBuffer() const {
    return bound_element_array_buffer_;
  }
  void SetElementArrayBuffer(WebGLBuffer*);

  WebGLBuffer* GetArrayBufferForAttrib(GLuint);
  void SetArrayBufferForAttrib(GLuint, WebGLBuffer*);
  void SetAttribEnabled(GLuint, bool);
  bool GetAttribEnabled(GLuint) const;
  bool IsAllEnabledAttribBufferBound() const {
    return is_all_enabled_attrib_buffer_bound_;
  }
  void UnbindBuffer(WebGLBuffer*);

  void Trace(blink::Visitor*) override;

 protected:
  WebGLVertexArrayObjectBase(WebGLRenderingContextBase*, VaoType);

 private:
  void DispatchDetached(gpu::gles2::GLES2Interface*);
  bool HasObject() const override { return object_ != 0; }
  void DeleteObjectImpl(gpu::gles2::GLES2Interface*) override;

  void UpdateAttribBufferBoundStatus();

  GLuint object_;

  VaoType type_;
  bool has_ever_been_bound_;
  TraceWrapperMember<WebGLBuffer> bound_element_array_buffer_;
  HeapVector<TraceWrapperMember<WebGLBuffer>> array_buffer_list_;
  Vector<bool> attrib_enabled_;
  bool is_all_enabled_attrib_buffer_bound_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_VERTEX_ARRAY_OBJECT_BASE_H_
