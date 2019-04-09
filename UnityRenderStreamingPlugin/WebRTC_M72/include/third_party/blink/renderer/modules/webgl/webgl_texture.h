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

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_TEXTURE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_TEXTURE_H_

#include "third_party/blink/public/platform/web_media_player.h"
#include "third_party/blink/renderer/modules/webgl/webgl_shared_platform_3d_object.h"

namespace blink {

class WebGLTexture final : public WebGLSharedPlatform3DObject {
  DEFINE_WRAPPERTYPEINFO();

 public:
  explicit WebGLTexture(WebGLRenderingContextBase*);
  ~WebGLTexture() override;

  static WebGLTexture* Create(WebGLRenderingContextBase*);

  void SetTarget(GLenum);

  GLenum GetTarget() const { return target_; }

  bool HasEverBeenBound() const { return Object() && target_; }

  static GLint ComputeLevelCount(GLsizei width, GLsizei height, GLsizei depth);

  int GetLastUploadedVideoFrameId() const {
    return last_uploaded_video_frame_metadata_.frame_id;
  }

  void UpdateLastUploadedFrame(
      blink::WebMediaPlayer::VideoFrameUploadMetadata frame_metadata) {
    last_uploaded_video_frame_metadata_ = frame_metadata;
  }

  void ClearLastUploadedFrame() { last_uploaded_video_frame_metadata_ = {}; }

  unsigned lastUploadedVideoWidth() const {
    return last_uploaded_video_frame_metadata_.visible_rect.width();
  }
  unsigned lastUploadedVideoHeight() const {
    return last_uploaded_video_frame_metadata_.visible_rect.height();
  }
  double lastUploadedVideoTimestamp() const {
    return last_uploaded_video_frame_metadata_.timestamp.InSecondsF();
  }
  bool lastUploadedVideoFrameWasSkipped() const {
    return last_uploaded_video_frame_metadata_.skipped;
  }

 private:
  void DeleteObjectImpl(gpu::gles2::GLES2Interface*) override;

  bool IsTexture() const override { return true; }

  int MapTargetToIndex(GLenum) const;

  GLenum target_;

  blink::WebMediaPlayer::VideoFrameUploadMetadata
      last_uploaded_video_frame_metadata_ = {};
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_TEXTURE_H_
