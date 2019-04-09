/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CODING_CODECS_H264_H264_COLOR_SPACE_H_
#define MODULES_VIDEO_CODING_CODECS_H264_H264_COLOR_SPACE_H_

#include "api/video/color_space.h"

extern "C" {
#include "third_party/ffmpeg/libavcodec/avcodec.h"
}  // extern "C"

namespace webrtc {

// Helper class for extracting color space information from H264 stream.
ColorSpace ExtractH264ColorSpace(AVCodecContext* codec);

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_CODECS_H264_H264_COLOR_SPACE_H_
