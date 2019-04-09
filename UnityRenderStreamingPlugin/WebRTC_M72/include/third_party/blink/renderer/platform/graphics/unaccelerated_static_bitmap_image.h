// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_UNACCELERATED_STATIC_BITMAP_IMAGE_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_UNACCELERATED_STATIC_BITMAP_IMAGE_H_

#include "base/memory/weak_ptr.h"
#include "base/single_thread_task_runner.h"
#include "base/threading/thread_checker.h"
#include "third_party/blink/renderer/platform/graphics/static_bitmap_image.h"
#include "third_party/blink/renderer/platform/scheduler/public/thread.h"

namespace blink {

class PLATFORM_EXPORT UnacceleratedStaticBitmapImage final
    : public StaticBitmapImage {
 public:
  ~UnacceleratedStaticBitmapImage() override;
  static scoped_refptr<UnacceleratedStaticBitmapImage> Create(sk_sp<SkImage>);
  static scoped_refptr<UnacceleratedStaticBitmapImage> Create(PaintImage);

  bool CurrentFrameKnownToBeOpaque() override;
  IntSize Size() const override;
  bool IsPremultiplied() const override;
  scoped_refptr<StaticBitmapImage> MakeAccelerated(
      base::WeakPtr<WebGraphicsContext3DProviderWrapper> context_wrapper)
      override;

  void Draw(cc::PaintCanvas*,
            const cc::PaintFlags&,
            const FloatRect& dst_rect,
            const FloatRect& src_rect,
            RespectImageOrientationEnum,
            ImageClampingMode,
            ImageDecodingMode) override;

  PaintImage PaintImageForCurrentFrame() override;

  void Transfer() final;

 private:
  UnacceleratedStaticBitmapImage(sk_sp<SkImage>);
  UnacceleratedStaticBitmapImage(PaintImage);

  PaintImage paint_image_;
  THREAD_CHECKER(thread_checker_);

  sk_sp<SkImage> original_skia_image_;
  scoped_refptr<base::SingleThreadTaskRunner> original_skia_image_task_runner_;
};

}  // namespace blink

#endif
