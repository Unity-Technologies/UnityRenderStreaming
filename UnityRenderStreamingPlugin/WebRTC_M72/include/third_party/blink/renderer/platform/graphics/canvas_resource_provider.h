// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_CANVAS_RESOURCE_PROVIDER_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_CANVAS_RESOURCE_PROVIDER_H_

#include "cc/paint/skia_paint_canvas.h"
#include "cc/raster/playback_image_provider.h"
#include "third_party/blink/renderer/platform/graphics/canvas_resource.h"
#include "third_party/skia/include/core/SkSurface.h"

class GrContext;
class SkCanvas;

namespace cc {
class ImageDecodeCache;
class PaintCanvas;
}

namespace gpu {
namespace gles2 {

class GLES2Interface;

}  // namespace gles2
}  // namespace gpu

namespace blink {

class CanvasResourceDispatcher;
class WebGraphicsContext3DProviderWrapper;

// CanvasResourceProvider
//==============================================================================
//
// This is an abstract base class that encapsulates a drawable graphics
// resource.  Subclasses manage specific resource types (Gpu Textures,
// GpuMemoryBuffer, Bitmap in RAM). CanvasResourceProvider serves as an
// abstraction layer for these resource types. It is designed to serve
// the needs of Canvas2DLayerBridge, but can also be used as a general purpose
// provider of drawable surfaces for 2D rendering with skia.
//
// General usage:
//   1) Use the Create() static method to create an instance
//   2) use Canvas() to get a drawing interface
//   3) Call Snapshot() to acquire a bitmap with the rendered image in it.

class PLATFORM_EXPORT CanvasResourceProvider
    : public WebGraphicsContext3DProviderWrapper::DestructionObserver {

 public:
  enum ResourceUsage {
    kSoftwareResourceUsage,
    kSoftwareCompositedResourceUsage,
    kAcceleratedResourceUsage,
    kAcceleratedCompositedResourceUsage,
  };

  enum PresentationMode {
    kDefaultPresentationMode,            // GPU Texture or shared memory bitmap
    kAllowImageChromiumPresentationMode  // Use CHROMIUM_image gl extension
  };

  // These values are persisted to logs. Entries should not be renumbered and
  // numeric values should never be reused.
  enum ResourceProviderType {
    kTexture = 0,
    kBitmap = 1,
    kSharedBitmap = 2,
    kTextureGpuMemoryBuffer = 3,
    kBitmapGpuMemoryBuffer = 4,
    kMaxValue = kBitmapGpuMemoryBuffer,
  };

  void static RecordTypeToUMA(ResourceProviderType type);

  static std::unique_ptr<CanvasResourceProvider> Create(
      const IntSize&,
      ResourceUsage,
      base::WeakPtr<WebGraphicsContext3DProviderWrapper>,
      unsigned msaa_sample_count,
      const CanvasColorParams&,
      PresentationMode,
      base::WeakPtr<CanvasResourceDispatcher>,
      bool is_origin_top_left = true);

  // Use Snapshot() for capturing a frame that is intended to be displayed via
  // the compositor. Cases that are destined to be transferred via a
  // TransferableResource should call ProduceFrame() instead.
  virtual scoped_refptr<CanvasResource> ProduceFrame() = 0;
  scoped_refptr<StaticBitmapImage> Snapshot();

  // WebGraphicsContext3DProvider::DestructionObserver implementation.
  void OnContextDestroyed() override;

  cc::PaintCanvas* Canvas();
  void ReleaseLockedImages();
  void FlushSkia() const;
  const CanvasColorParams& ColorParams() const { return color_params_; }
  void SetFilterQuality(SkFilterQuality quality) { filter_quality_ = quality; }
  const IntSize& Size() const { return size_; }
  virtual bool IsValid() const = 0;
  virtual bool IsAccelerated() const = 0;
  virtual bool SupportsDirectCompositing() const = 0;
  virtual bool SupportsSingleBuffering() const { return false; }
  uint32_t ContentUniqueID() const;
  CanvasResourceDispatcher* ResourceDispatcher() {
    return resource_dispatcher_.get();
  }

  // Indicates that the compositing path is single buffered, meaning that
  // ProduceFrame() return a reference to the same resource each time, which
  // implies that Producing an animation frame may overwrite the resource used
  // by the previous frame. This results in graphics updates skipping the queue,
  // thus reducing latency, but with the possible side effects of tearing (in
  // cases where the resource is scanned out directly) and irregular frame rate.
  bool IsSingleBuffered() { return !resource_recycling_enabled_; }

  // Attempt to enable single buffering mode on this resource provider.  May
  // fail if the CanvasResourcePRovider subclass does not support this mode of
  // operation.
  void TryEnableSingleBuffering();

  void RecycleResource(scoped_refptr<CanvasResource>);
  void SetResourceRecyclingEnabled(bool);
  void ClearRecycledResources();
  scoped_refptr<CanvasResource> NewOrRecycledResource();

  SkSurface* GetSkSurface() const;
  bool IsGpuContextLost() const;
  bool WritePixels(const SkImageInfo& orig_info,
                   const void* pixels,
                   size_t row_bytes,
                   int x,
                   int y);
  virtual GLuint GetBackingTextureHandleForOverwrite() {
    NOTREACHED();
    return 0;
  }
  virtual void* GetPixelBufferAddressForOverwrite() {
    NOTREACHED();
    return nullptr;
  }
  void Clear();
  ~CanvasResourceProvider() override;

 protected:
  gpu::gles2::GLES2Interface* ContextGL() const;
  GrContext* GetGrContext() const;
  base::WeakPtr<WebGraphicsContext3DProviderWrapper> ContextProviderWrapper() {
    return context_provider_wrapper_;
  }
  SkFilterQuality FilterQuality() const { return filter_quality_; }
  base::WeakPtr<CanvasResourceProvider> CreateWeakPtr() {
    return weak_ptr_factory_.GetWeakPtr();
  }

  CanvasResourceProvider(const IntSize&,
                         const CanvasColorParams&,
                         base::WeakPtr<WebGraphicsContext3DProviderWrapper>,
                         base::WeakPtr<CanvasResourceDispatcher>);

  // Its important to use this method for generating PaintImage wrapped canvas
  // snapshots to get a cache hit from cc's ImageDecodeCache. This method
  // ensures that the PaintImage ID for the snapshot, used for keying
  // decodes/uploads in the cache is invalidated only when the canvas contents
  // change.
  cc::PaintImage MakeImageSnapshot();

 private:
  class CanvasImageProvider;

  virtual sk_sp<SkSurface> CreateSkSurface() const = 0;
  virtual scoped_refptr<CanvasResource> CreateResource();
  cc::ImageDecodeCache* ImageDecodeCacheRGBA8();
  cc::ImageDecodeCache* ImageDecodeCacheF16();

  base::WeakPtr<WebGraphicsContext3DProviderWrapper> context_provider_wrapper_;
  base::WeakPtr<CanvasResourceDispatcher> resource_dispatcher_;
  IntSize size_;
  CanvasColorParams color_params_;
  std::unique_ptr<CanvasImageProvider> canvas_image_provider_;
  std::unique_ptr<cc::SkiaPaintCanvas> canvas_;
  mutable sk_sp<SkSurface> surface_;  // mutable for lazy init
  std::unique_ptr<SkCanvas> xform_canvas_;
  SkFilterQuality filter_quality_ = kLow_SkFilterQuality;

  const cc::PaintImage::Id snapshot_paint_image_id_;
  cc::PaintImage::ContentId snapshot_paint_image_content_id_ =
      cc::PaintImage::kInvalidContentId;
  uint32_t snapshot_sk_image_id_ = 0u;

  // When and if |resource_recycling_enabled_| is false, |canvas_resources_|
  // will only hold one CanvasResource at most.
  WTF::Vector<scoped_refptr<CanvasResource>> canvas_resources_;
  bool resource_recycling_enabled_ = true;

  base::WeakPtrFactory<CanvasResourceProvider> weak_ptr_factory_;

  DISALLOW_COPY_AND_ASSIGN(CanvasResourceProvider);
};

}  // namespace blink

#endif
