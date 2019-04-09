// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_GPU_DRAWING_BUFFER_TEST_HELPERS_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_GPU_DRAWING_BUFFER_TEST_HELPERS_H_

#include "build/build_config.h"
#include "cc/test/stub_decode_cache.h"
#include "gpu/command_buffer/common/capabilities.h"
#include "gpu/config/gpu_feature_info.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/public/platform/web_graphics_context_3d_provider.h"
#include "third_party/blink/renderer/platform/graphics/canvas_color_params.h"
#include "third_party/blink/renderer/platform/graphics/gpu/drawing_buffer.h"
#include "third_party/blink/renderer/platform/graphics/gpu/extensions_3d_util.h"
#include "third_party/blink/renderer/platform/runtime_enabled_features.h"

namespace blink {

enum {
  kInitialWidth = 100,
  kInitialHeight = 100,
  kAlternateHeight = 50,
};

enum UseMultisampling {
  kDisableMultisampling,
  kEnableMultisampling,
};

class WebGraphicsContext3DProviderForTests
    : public WebGraphicsContext3DProvider {
 public:
  WebGraphicsContext3DProviderForTests(
      std::unique_ptr<gpu::gles2::GLES2Interface> gl)
      : gl_(std::move(gl)) {}

  gpu::gles2::GLES2Interface* ContextGL() override { return gl_.get(); }

  // Not used by WebGL code.
  GrContext* GetGrContext() override { return nullptr; }
  gpu::webgpu::WebGPUInterface* WebGPUInterface() override { return nullptr; }
  bool BindToCurrentThread() override { return false; }
  const gpu::Capabilities& GetCapabilities() const override {
    return capabilities_;
  }
  const gpu::GpuFeatureInfo& GetGpuFeatureInfo() const override {
    return gpu_feature_info_;
  }
  viz::GLHelper* GetGLHelper() override { return nullptr; }
  void SetLostContextCallback(base::Closure) override {}
  void SetErrorMessageCallback(
      base::RepeatingCallback<void(const char*, int32_t id)>) override {}
  cc::ImageDecodeCache* ImageDecodeCache(
      SkColorType color_type,
      sk_sp<SkColorSpace> color_space) override {
    return &image_decode_cache_;
  }

 private:
  cc::StubDecodeCache image_decode_cache_;
  std::unique_ptr<gpu::gles2::GLES2Interface> gl_;
  gpu::Capabilities capabilities_;
  gpu::GpuFeatureInfo gpu_feature_info_;
};

// The target to use when binding a texture to a Chromium image.
GLenum ImageCHROMIUMTextureTarget() {
#if defined(OS_MACOSX)
  return GC3D_TEXTURE_RECTANGLE_ARB;
#else
  return GL_TEXTURE_2D;
#endif
}

// The target to use when preparing a mailbox texture.
GLenum DrawingBufferTextureTarget() {
  if (RuntimeEnabledFeatures::WebGLImageChromiumEnabled())
    return ImageCHROMIUMTextureTarget();
  return GL_TEXTURE_2D;
}

class GLES2InterfaceForTests : public gpu::gles2::GLES2InterfaceStub,
                               public DrawingBuffer::Client {
 public:
  // GLES2InterfaceStub implementation:
  void BindTexture(GLenum target, GLuint texture) override {
    if (target == GL_TEXTURE_2D)
      state_.active_texture2d_binding = texture;
    bound_textures_[target] = texture;
  }

  void BindFramebuffer(GLenum target, GLuint framebuffer) override {
    switch (target) {
      case GL_FRAMEBUFFER:
        state_.draw_framebuffer_binding = framebuffer;
        state_.read_framebuffer_binding = framebuffer;
        break;
      case GL_DRAW_FRAMEBUFFER:
        state_.draw_framebuffer_binding = framebuffer;
        break;
      case GL_READ_FRAMEBUFFER:
        state_.read_framebuffer_binding = framebuffer;
        break;
      default:
        break;
    }
  }

  void BindRenderbuffer(GLenum target, GLuint renderbuffer) override {
    state_.renderbuffer_binding = renderbuffer;
  }

  void Enable(GLenum cap) override {
    if (cap == GL_SCISSOR_TEST)
      state_.scissor_enabled = true;
  }

  void Disable(GLenum cap) override {
    if (cap == GL_SCISSOR_TEST)
      state_.scissor_enabled = false;
  }

  void ClearColor(GLfloat red,
                  GLfloat green,
                  GLfloat blue,
                  GLfloat alpha) override {
    state_.clear_color[0] = red;
    state_.clear_color[1] = green;
    state_.clear_color[2] = blue;
    state_.clear_color[3] = alpha;
  }

  void ClearDepthf(GLfloat depth) override { state_.clear_depth = depth; }

  void ClearStencil(GLint s) override { state_.clear_stencil = s; }

  void ColorMask(GLboolean red,
                 GLboolean green,
                 GLboolean blue,
                 GLboolean alpha) override {
    state_.color_mask[0] = red;
    state_.color_mask[1] = green;
    state_.color_mask[2] = blue;
    state_.color_mask[3] = alpha;
  }

  void DepthMask(GLboolean flag) override { state_.depth_mask = flag; }

  void StencilMask(GLuint mask) override { state_.stencil_mask = mask; }

  void StencilMaskSeparate(GLenum face, GLuint mask) override {
    if (face == GL_FRONT)
      state_.stencil_mask = mask;
  }

  void PixelStorei(GLenum pname, GLint param) override {
    if (pname == GL_PACK_ALIGNMENT)
      state_.pack_alignment = param;
  }

  void BindBuffer(GLenum target, GLuint buffer) override {
    switch (target) {
      case GL_PIXEL_UNPACK_BUFFER:
        state_.pixel_unpack_buffer_binding = buffer;
        break;
      case GL_PIXEL_PACK_BUFFER:
        state_.pixel_pack_buffer_binding = buffer;
        break;
      default:
        break;
    }
  }

  void WaitSyncTokenCHROMIUM(const GLbyte* sync_token) override {
    memcpy(&most_recently_waited_sync_token_, sync_token,
           sizeof(most_recently_waited_sync_token_));
  }

  GLenum CheckFramebufferStatus(GLenum target) override {
    return GL_FRAMEBUFFER_COMPLETE;
  }

  void GetIntegerv(GLenum pname, GLint* value) override {
    switch (pname) {
      case GL_DRAW_FRAMEBUFFER_BINDING:
        *value = state_.draw_framebuffer_binding;
        break;
      case GL_READ_FRAMEBUFFER_BINDING:
        *value = state_.read_framebuffer_binding;
        break;
      case GL_MAX_TEXTURE_SIZE:
        *value = 1024;
        break;
      default:
        break;
    }
  }

  void ProduceTextureDirectCHROMIUM(GLuint texture, GLbyte* mailbox) override {
    ++current_mailbox_byte_;
    memset(mailbox, current_mailbox_byte_, GL_MAILBOX_SIZE_CHROMIUM);
    if (!create_image_chromium_fail_) {
      ASSERT_TRUE(texture_sizes_.Contains(texture));
      most_recently_produced_size_ = texture_sizes_.at(texture);
    }
  }

  void TexImage2D(GLenum target,
                  GLint level,
                  GLint internalformat,
                  GLsizei width,
                  GLsizei height,
                  GLint border,
                  GLenum format,
                  GLenum type,
                  const void* pixels) override {
    if (target == GL_TEXTURE_2D && !level) {
      texture_sizes_.Set(bound_textures_[target], IntSize(width, height));
    }
  }

  GLuint CreateImageCHROMIUM(ClientBuffer buffer,
                             GLsizei width,
                             GLsizei height,
                             GLenum internalformat) override {
    if (create_image_chromium_fail_)
      return 0;
    image_sizes_.Set(current_image_id_, IntSize(width, height));
    return current_image_id_++;
  }

  MOCK_METHOD1(DestroyImageMock, void(GLuint imageId));
  void DestroyImageCHROMIUM(GLuint image_id) override {
    image_sizes_.erase(image_id);
    // No textures should be bound to this.
    CHECK(image_to_texture_map_.find(image_id) == image_to_texture_map_.end());
    image_sizes_.erase(image_id);
    DestroyImageMock(image_id);
  }

  MOCK_METHOD1(BindTexImage2DMock, void(GLint imageId));
  void BindTexImage2DCHROMIUM(GLenum target, GLint image_id) override {
    if (target == ImageCHROMIUMTextureTarget()) {
      texture_sizes_.Set(bound_textures_[target],
                         image_sizes_.find(image_id)->value);
      image_to_texture_map_.Set(image_id, bound_textures_[target]);
      BindTexImage2DMock(image_id);
    }
  }

  MOCK_METHOD1(ReleaseTexImage2DMock, void(GLint imageId));
  void ReleaseTexImage2DCHROMIUM(GLenum target, GLint image_id) override {
    if (target == ImageCHROMIUMTextureTarget()) {
      image_sizes_.Set(current_image_id_, IntSize());
      image_to_texture_map_.erase(image_id);
      ReleaseTexImage2DMock(image_id);
    }
  }

  void GenSyncTokenCHROMIUM(GLbyte* sync_token) override {
    static uint64_t unique_id = 1;
    gpu::SyncToken source(
        gpu::GPU_IO, gpu::CommandBufferId::FromUnsafeValue(unique_id++), 2);
    memcpy(sync_token, &source, sizeof(source));
  }

  void GenTextures(GLsizei n, GLuint* textures) override {
    static GLuint id = 1;
    for (GLsizei i = 0; i < n; ++i)
      textures[i] = id++;
  }

  // DrawingBuffer::Client implementation.
  bool DrawingBufferClientIsBoundForDraw() override {
    return !state_.draw_framebuffer_binding;
  }
  void DrawingBufferClientRestoreScissorTest() override {
    state_.scissor_enabled = saved_state_.scissor_enabled;
  }
  void DrawingBufferClientRestoreMaskAndClearValues() override {
    memcpy(state_.color_mask, saved_state_.color_mask,
           sizeof(state_.color_mask));
    state_.clear_depth = saved_state_.clear_depth;
    state_.clear_stencil = saved_state_.clear_stencil;

    memcpy(state_.clear_color, saved_state_.clear_color,
           sizeof(state_.clear_color));
    state_.depth_mask = saved_state_.depth_mask;
    state_.stencil_mask = saved_state_.stencil_mask;
  }
  void DrawingBufferClientRestorePixelPackParameters() override {
    // TODO(zmo): restore ES3 pack parameters?
    state_.pack_alignment = saved_state_.pack_alignment;
  }
  void DrawingBufferClientRestoreTexture2DBinding() override {
    state_.active_texture2d_binding = saved_state_.active_texture2d_binding;
  }
  void DrawingBufferClientRestoreRenderbufferBinding() override {
    state_.renderbuffer_binding = saved_state_.renderbuffer_binding;
  }
  void DrawingBufferClientRestoreFramebufferBinding() override {
    state_.draw_framebuffer_binding = saved_state_.draw_framebuffer_binding;
    state_.read_framebuffer_binding = saved_state_.read_framebuffer_binding;
  }
  void DrawingBufferClientRestorePixelUnpackBufferBinding() override {
    state_.pixel_unpack_buffer_binding =
        saved_state_.pixel_unpack_buffer_binding;
  }
  void DrawingBufferClientRestorePixelPackBufferBinding() override {
    state_.pixel_pack_buffer_binding = saved_state_.pixel_pack_buffer_binding;
  }

  // Testing methods.
  gpu::SyncToken MostRecentlyWaitedSyncToken() const {
    return most_recently_waited_sync_token_;
  }
  GLuint NextImageIdToBeCreated() const { return current_image_id_; }
  IntSize MostRecentlyProducedSize() const {
    return most_recently_produced_size_;
  }

  void SetCreateImageChromiumFail(bool fail) {
    create_image_chromium_fail_ = fail;
  }

  // Saves current GL state for later verification.
  void SaveState() { saved_state_ = state_; }
  void VerifyStateHasNotChangedSinceSave() const {
    for (size_t i = 0; i < 4; ++i) {
      EXPECT_EQ(state_.clear_color[0], saved_state_.clear_color[0]);
      EXPECT_EQ(state_.color_mask[0], saved_state_.color_mask[0]);
    }
    EXPECT_EQ(state_.clear_depth, saved_state_.clear_depth);
    EXPECT_EQ(state_.clear_stencil, saved_state_.clear_stencil);
    EXPECT_EQ(state_.depth_mask, saved_state_.depth_mask);
    EXPECT_EQ(state_.stencil_mask, saved_state_.stencil_mask);
    EXPECT_EQ(state_.pack_alignment, saved_state_.pack_alignment);
    EXPECT_EQ(state_.active_texture2d_binding,
              saved_state_.active_texture2d_binding);
    EXPECT_EQ(state_.renderbuffer_binding, saved_state_.renderbuffer_binding);
    EXPECT_EQ(state_.draw_framebuffer_binding,
              saved_state_.draw_framebuffer_binding);
    EXPECT_EQ(state_.read_framebuffer_binding,
              saved_state_.read_framebuffer_binding);
    EXPECT_EQ(state_.pixel_unpack_buffer_binding,
              saved_state_.pixel_unpack_buffer_binding);
    EXPECT_EQ(state_.pixel_pack_buffer_binding,
              saved_state_.pixel_pack_buffer_binding);
  }

 private:
  std::map<GLenum, GLuint> bound_textures_;

  // State tracked to verify that it is restored correctly.
  struct State {
    bool scissor_enabled = false;

    GLfloat clear_color[4] = {0, 0, 0, 0};
    GLfloat clear_depth = 0;
    GLint clear_stencil = 0;

    GLboolean color_mask[4] = {0, 0, 0, 0};
    GLboolean depth_mask = 0;
    GLuint stencil_mask = 0;

    GLint pack_alignment = 4;

    // The bound 2D texture for the active texture unit.
    GLuint active_texture2d_binding = 0;
    GLuint renderbuffer_binding = 0;
    GLuint draw_framebuffer_binding = 0;
    GLuint read_framebuffer_binding = 0;
    GLuint pixel_unpack_buffer_binding = 0;
    GLuint pixel_pack_buffer_binding = 0;
  };
  State state_;
  State saved_state_;

  gpu::SyncToken most_recently_waited_sync_token_;
  GLbyte current_mailbox_byte_ = 0;
  IntSize most_recently_produced_size_;
  bool create_image_chromium_fail_ = false;
  GLuint current_image_id_ = 1;
  HashMap<GLuint, IntSize> texture_sizes_;
  HashMap<GLuint, IntSize> image_sizes_;
  HashMap<GLuint, GLuint> image_to_texture_map_;
};

class DrawingBufferForTests : public DrawingBuffer {
 public:
  static scoped_refptr<DrawingBufferForTests> Create(
      std::unique_ptr<WebGraphicsContext3DProvider> context_provider,
      bool using_gpu_compositing,
      DrawingBuffer::Client* client,
      const IntSize& size,
      PreserveDrawingBuffer preserve,
      UseMultisampling use_multisampling) {
    std::unique_ptr<Extensions3DUtil> extensions_util =
        Extensions3DUtil::Create(context_provider->ContextGL());
    scoped_refptr<DrawingBufferForTests> drawing_buffer =
        base::AdoptRef(new DrawingBufferForTests(
            std::move(context_provider), using_gpu_compositing,
            std::move(extensions_util), client, preserve));
    if (!drawing_buffer->Initialize(
            size, use_multisampling != kDisableMultisampling)) {
      drawing_buffer->BeginDestruction();
      return nullptr;
    }
    return drawing_buffer;
  }

  DrawingBufferForTests(
      std::unique_ptr<WebGraphicsContext3DProvider> context_provider,
      bool using_gpu_compositing,
      std::unique_ptr<Extensions3DUtil> extensions_util,
      DrawingBuffer::Client* client,
      PreserveDrawingBuffer preserve)
      : DrawingBuffer(
            std::move(context_provider),
            using_gpu_compositing,
            std::move(extensions_util),
            client,
            false /* discardFramebufferSupported */,
            true /* wantAlphaChannel */,
            true /* premultipliedAlpha */,
            preserve,
            kWebGL1,
            false /* wantDepth */,
            false /* wantStencil */,
            DrawingBuffer::kAllowChromiumImage /* ChromiumImageUsage */,
            CanvasColorParams()),
        live_(nullptr) {}

  ~DrawingBufferForTests() override {
    if (live_)
      *live_ = false;
  }

  GLES2InterfaceForTests* ContextGLForTests() {
    return static_cast<GLES2InterfaceForTests*>(ContextGL());
  }

  bool* live_;

  int RecycledBitmapCount() { return recycled_bitmaps_.size(); }
};

}  // blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_GPU_DRAWING_BUFFER_TEST_HELPERS_H_
