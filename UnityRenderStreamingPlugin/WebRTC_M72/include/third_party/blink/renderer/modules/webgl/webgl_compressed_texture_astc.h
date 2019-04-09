// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_COMPRESSED_TEXTURE_ASTC_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_COMPRESSED_TEXTURE_ASTC_H_

#include "third_party/blink/renderer/modules/webgl/webgl_extension.h"

namespace blink {

class WebGLCompressedTextureASTC final : public WebGLExtension {
  DEFINE_WRAPPERTYPEINFO();

 public:
  typedef struct {
    int compress_type;
    int block_width;
    int block_height;
  } BlockSizeCompressASTC;

  static WebGLCompressedTextureASTC* Create(WebGLRenderingContextBase*);
  static bool Supported(WebGLRenderingContextBase*);
  static const char* ExtensionName();

  explicit WebGLCompressedTextureASTC(WebGLRenderingContextBase*);

  WebGLExtensionName GetName() const override;
  static const BlockSizeCompressASTC kBlockSizeCompressASTC[];
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_COMPRESSED_TEXTURE_ASTC_H_
