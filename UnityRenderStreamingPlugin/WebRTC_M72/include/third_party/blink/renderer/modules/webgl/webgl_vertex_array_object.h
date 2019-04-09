// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_VERTEX_ARRAY_OBJECT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_VERTEX_ARRAY_OBJECT_H_

#include "third_party/blink/renderer/modules/webgl/webgl_vertex_array_object_base.h"

namespace blink {

class WebGLVertexArrayObject final : public WebGLVertexArrayObjectBase {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static WebGLVertexArrayObject* Create(WebGLRenderingContextBase*, VaoType);

  explicit WebGLVertexArrayObject(WebGLRenderingContextBase*, VaoType);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_VERTEX_ARRAY_OBJECT_H_
