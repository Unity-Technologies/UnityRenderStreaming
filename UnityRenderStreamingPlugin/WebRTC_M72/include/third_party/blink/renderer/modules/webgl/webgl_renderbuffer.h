/*
 * Copyright (C) 2009 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_RENDERBUFFER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_RENDERBUFFER_H_

#include "base/memory/scoped_refptr.h"
#include "third_party/blink/renderer/modules/webgl/webgl_shared_platform_3d_object.h"

namespace blink {

class WebGLRenderbuffer final : public WebGLSharedPlatform3DObject {
  DEFINE_WRAPPERTYPEINFO();

 public:
  explicit WebGLRenderbuffer(WebGLRenderingContextBase*);
  ~WebGLRenderbuffer() override;

  static WebGLRenderbuffer* Create(WebGLRenderingContextBase*);

  void SetInternalFormat(GLenum internalformat) {
    internal_format_ = internalformat;
  }
  GLenum InternalFormat() const { return internal_format_; }

  void SetSize(GLsizei width, GLsizei height) {
    width_ = width;
    height_ = height;
  }
  GLsizei Width() const { return width_; }
  GLsizei Height() const { return height_; }

  bool HasEverBeenBound() const { return Object() && has_ever_been_bound_; }

  void SetHasEverBeenBound() { has_ever_been_bound_ = true; }

  void Trace(blink::Visitor*) override;

 protected:
  void DeleteObjectImpl(gpu::gles2::GLES2Interface*) override;

 private:
  bool IsRenderbuffer() const override { return true; }

  GLenum internal_format_;
  GLsizei width_, height_;

  bool has_ever_been_bound_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_RENDERBUFFER_H_
