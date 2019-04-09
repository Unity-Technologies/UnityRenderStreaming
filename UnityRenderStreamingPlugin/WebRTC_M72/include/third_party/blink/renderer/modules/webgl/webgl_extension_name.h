// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_EXTENSION_NAME_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_EXTENSION_NAME_H_

namespace blink {

// Extension names are needed to properly wrap instances in JavaScript objects.
enum WebGLExtensionName {
  kANGLEInstancedArraysName,
  kEXTBlendMinMaxName,
  kEXTColorBufferFloatName,
  kEXTColorBufferHalfFloatName,
  kEXTDisjointTimerQueryName,
  kEXTDisjointTimerQueryWebGL2Name,
  kEXTFragDepthName,
  kEXTShaderTextureLODName,
  kEXTsRGBName,
  kKHRParallelShaderCompileName,
  kEXTTextureFilterAnisotropicName,
  kOESElementIndexUintName,
  kOESStandardDerivativesName,
  kOESTextureFloatLinearName,
  kOESTextureFloatName,
  kOESTextureHalfFloatLinearName,
  kOESTextureHalfFloatName,
  kOESVertexArrayObjectName,
  kWebGLColorBufferFloatName,
  kWebGLCompressedTextureASTCName,
  kWebGLCompressedTextureETCName,
  kWebGLCompressedTextureETC1Name,
  kWebGLCompressedTexturePVRTCName,
  kWebGLCompressedTextureS3TCName,
  kWebGLCompressedTextureS3TCsRGBName,
  kWebGLDebugRendererInfoName,
  kWebGLDebugShadersName,
  kWebGLDepthTextureName,
  kWebGLDrawBuffersName,
  kWebGLGetBufferSubDataAsyncName,
  kWebGLLoseContextName,
  kWebGLMultiviewName,
  kWebGLExtensionNameCount,  // Must be the last entry
};
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_WEBGL_EXTENSION_NAME_H_
