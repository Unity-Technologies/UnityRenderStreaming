// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_GPU_WEBGL_IMAGE_CONVERSION_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_GPU_WEBGL_IMAGE_CONVERSION_H_

#include "base/macros.h"
#include "base/memory/scoped_refptr.h"
#include "base/optional.h"
#include "third_party/blink/renderer/platform/graphics/image.h"
#include "third_party/blink/renderer/platform/graphics/skia/image_pixel_locker.h"
#include "third_party/blink/renderer/platform/heap/heap.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/khronos/GLES2/gl2.h"
#include "third_party/khronos/GLES2/gl2ext.h"
#include "third_party/khronos/GLES3/gl3.h"

namespace blink {
class Image;
class IntSize;

// Helper functions for texture uploading and pixel readback.
class PLATFORM_EXPORT WebGLImageConversion final {
  STATIC_ONLY(WebGLImageConversion);

 public:
  // Attempt to enumerate all possible native image formats to
  // reduce the amount of temporary allocations during texture
  // uploading. This enum must be public because it is accessed
  // by non-member functions.
  // "_S" postfix indicates signed type.
  enum DataFormat {
    kDataFormatRGBA8 = 0,
    kDataFormatRGBA8_S,
    kDataFormatRGBA16,
    kDataFormatRGBA16_S,
    kDataFormatRGBA32,
    kDataFormatRGBA32_S,
    kDataFormatRGBA16F,
    kDataFormatRGBA32F,
    kDataFormatRGBA2_10_10_10,
    kDataFormatRGB8,
    kDataFormatRGB8_S,
    kDataFormatRGB16,
    kDataFormatRGB16_S,
    kDataFormatRGB32,
    kDataFormatRGB32_S,
    kDataFormatRGB16F,
    kDataFormatRGB32F,
    kDataFormatBGR8,
    kDataFormatBGRA8,
    kDataFormatARGB8,
    kDataFormatABGR8,
    kDataFormatRGBA5551,
    kDataFormatRGBA4444,
    kDataFormatRGB565,
    kDataFormatRGB10F11F11F,
    kDataFormatRGB5999,
    kDataFormatRG8,
    kDataFormatRG8_S,
    kDataFormatRG16,
    kDataFormatRG16_S,
    kDataFormatRG32,
    kDataFormatRG32_S,
    kDataFormatRG16F,
    kDataFormatRG32F,
    kDataFormatR8,
    kDataFormatR8_S,
    kDataFormatR16,
    kDataFormatR16_S,
    kDataFormatR32,
    kDataFormatR32_S,
    kDataFormatR16F,
    kDataFormatR32F,
    kDataFormatRA8,
    kDataFormatRA16F,
    kDataFormatRA32F,
    kDataFormatAR8,
    kDataFormatA8,
    kDataFormatA16F,
    kDataFormatA32F,
    kDataFormatD16,
    kDataFormatD32,
    kDataFormatD32F,
    kDataFormatDS24_8,
    kDataFormatNumFormats
  };

  enum ChannelBits {
    kChannelRed = 1,
    kChannelGreen = 2,
    kChannelBlue = 4,
    kChannelAlpha = 8,
    kChannelDepth = 16,
    kChannelStencil = 32,
    kChannelRG = kChannelRed | kChannelGreen,
    kChannelRGB = kChannelRed | kChannelGreen | kChannelBlue,
    kChannelRGBA = kChannelRGB | kChannelAlpha,
    kChannelDepthStencil = kChannelDepth | kChannelStencil,
  };

  // Possible alpha operations that may need to occur during
  // pixel packing. FIXME: kAlphaDoUnmultiply is lossy and must
  // be removed.
  enum AlphaOp {
    kAlphaDoNothing = 0,
    kAlphaDoPremultiply = 1,
    kAlphaDoUnmultiply = 2
  };

  enum ImageHtmlDomSource {
    kHtmlDomImage = 0,
    kHtmlDomCanvas = 1,
    kHtmlDomVideo = 2,
    kHtmlDomNone = 3
  };

  struct PLATFORM_EXPORT PixelStoreParams final {
    PixelStoreParams();

    GLint alignment;
    GLint row_length;
    GLint image_height;
    GLint skip_pixels;
    GLint skip_rows;
    GLint skip_images;
  };

  class PLATFORM_EXPORT ImageExtractor final {
    STACK_ALLOCATED();

   public:
    ImageExtractor(Image*,
                   ImageHtmlDomSource,
                   bool premultiply_alpha,
                   bool ignore_color_space);

    const void* ImagePixelData() {
      return image_pixel_locker_ ? image_pixel_locker_->Pixels() : nullptr;
    }
    unsigned ImageWidth() { return image_width_; }
    unsigned ImageHeight() { return image_height_; }
    DataFormat ImageSourceFormat() { return image_source_format_; }
    AlphaOp ImageAlphaOp() { return alpha_op_; }
    unsigned ImageSourceUnpackAlignment() {
      return image_source_unpack_alignment_;
    }

   private:
    // Extracts the image and keeps track of its status, such as width, height,
    // Source Alignment, format, AlphaOp, etc. This needs to lock the resources
    // or relevant data if needed.
    void ExtractImage(bool premultiply_alpha, bool ignore_color_space);

    Image* image_;
    base::Optional<ImagePixelLocker> image_pixel_locker_;
    ImageHtmlDomSource image_html_dom_source_;
    unsigned image_width_;
    unsigned image_height_;
    DataFormat image_source_format_;
    AlphaOp alpha_op_;
    unsigned image_source_unpack_alignment_;

    DISALLOW_COPY_AND_ASSIGN(ImageExtractor);
  };

  // Computes the components per pixel and bytes per component
  // for the given format and type combination. Returns false if
  // either was an invalid enum.
  static bool ComputeFormatAndTypeParameters(GLenum format,
                                             GLenum type,
                                             unsigned* components_per_pixel,
                                             unsigned* bytes_per_component);

  // Computes the image size in bytes. If paddingInBytes is not null, padding
  // is also calculated in return. Returns NO_ERROR if succeed, otherwise
  // return the suggested GL error indicating the cause of the failure:
  //   INVALID_VALUE if width/height/depth is negative or overflow happens.
  //   INVALID_ENUM if format/type is illegal.
  // Note that imageSizeBytes does not include skipSizeInBytes, but it is
  // guaranteed if NO_ERROR is returned, adding the two sizes won't cause
  // overflow.
  // |paddingInBytes| and |skipSizeInBytes| are optional and can be null, but
  // the overflow validation is still performed.
  static GLenum ComputeImageSizeInBytes(GLenum format,
                                        GLenum type,
                                        GLsizei width,
                                        GLsizei height,
                                        GLsizei depth,
                                        const PixelStoreParams&,
                                        unsigned* image_size_in_bytes,
                                        unsigned* padding_in_bytes,
                                        unsigned* skip_size_in_bytes);

  // Check if the format is one of the formats from the ImageData or DOM
  // elements. The format from ImageData is always RGBA8. The formats from DOM
  // elements vary with Graphics ports, but can only be RGBA8 or BGRA8.
  static ALWAYS_INLINE bool SrcFormatComeFromDOMElementOrImageData(
      DataFormat src_format) {
    return src_format == kDataFormatBGRA8 || src_format == kDataFormatRGBA8;
  }

  // The input can be either format or internalformat.
  static unsigned GetChannelBitsByFormat(GLenum);

  // The Following functions are implemented in
  // GraphicsContext3DImagePacking.cpp.

  // Packs the contents of the given Image, which is passed in |pixels|, into
  // the passed Vector according to the given format and type, and obeying the
  // flipY and AlphaOp flags. Returns true upon success.
  static bool PackImageData(Image*,
                            const void* pixels,
                            GLenum format,
                            GLenum type,
                            bool flip_y,
                            AlphaOp,
                            DataFormat source_format,
                            unsigned source_image_width,
                            unsigned source_image_height,
                            const IntRect& source_image_sub_rectangle,
                            int depth,
                            unsigned source_unpack_alignment,
                            int unpack_image_height,
                            Vector<uint8_t>& data);

  // Extracts the contents of the given ImageData into the passed Vector,
  // packing the pixel data according to the given format and type,
  // and obeying the flipY and premultiplyAlpha flags. Returns true
  // upon success.
  static bool ExtractImageData(const uint8_t* image_data,
                               DataFormat source_data_format,
                               const IntSize& image_data_size,
                               const IntRect& source_image_sub_rectangle,
                               int depth,
                               int unpack_image_height,
                               GLenum format,
                               GLenum type,
                               bool flip_y,
                               bool premultiply_alpha,
                               Vector<uint8_t>& data);

  // Helper function which extracts the user-supplied texture
  // data, applying the flipY and premultiplyAlpha parameters.
  // If the data is not tightly packed according to the passed
  // unpackAlignment, the output data will be tightly packed.
  // Returns true if successful, false if any error occurred.
  static bool ExtractTextureData(unsigned width,
                                 unsigned height,
                                 GLenum format,
                                 GLenum type,
                                 const PixelStoreParams& unpack_params,
                                 bool flip_y,
                                 bool premultiply_alpha,
                                 const void* pixels,
                                 Vector<uint8_t>& data);

  // End GraphicsContext3DImagePacking.cpp functions

 private:
  friend class WebGLImageConversionTest;
  // Helper for packImageData/extractImageData/extractTextureData, which
  // implement packing of pixel data into the specified OpenGL destination
  // format and type. A sourceUnpackAlignment of zero indicates that the source
  // data is tightly packed. Non-zero values may take a slow path. Destination
  // data will have no gaps between rows. Implemented in
  // GraphicsContext3DImagePacking.cpp.
  static bool PackPixels(const uint8_t* source_data,
                         DataFormat source_data_format,
                         unsigned source_data_width,
                         unsigned source_data_height,
                         const IntRect& source_data_sub_rectangle,
                         int depth,
                         unsigned source_unpack_alignment,
                         int unpack_image_height,
                         unsigned destination_format,
                         unsigned destination_type,
                         AlphaOp,
                         void* destination_data,
                         bool flip_y);
  static void UnpackPixels(const uint16_t* source_data,
                           DataFormat source_data_format,
                           unsigned pixels_per_row,
                           uint8_t* destination_data);
  static void PackPixels(const uint8_t* source_data,
                         DataFormat source_data_format,
                         unsigned pixels_per_row,
                         uint8_t* destination_data);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_GPU_WEBGL_IMAGE_CONVERSION_H_
