// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIASTREAM_INPUT_DEVICE_INFO_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIASTREAM_INPUT_DEVICE_INFO_H_

#include "third_party/blink/public/platform/web_media_stream_source.h"
#include "third_party/blink/renderer/modules/mediastream/media_device_info.h"

namespace blink {

class MediaTrackCapabilities;

class InputDeviceInfo final : public MediaDeviceInfo {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static InputDeviceInfo* Create(const String& device_id,
                                 const String& label,
                                 const String& group_id,
                                 MediaDeviceType);

  InputDeviceInfo(const String& device_id,
                  const String& label,
                  const String& group_id,
                  MediaDeviceType);

  void SetVideoInputCapabilities(mojom::blink::VideoInputDeviceCapabilitiesPtr);

  MediaTrackCapabilities* getCapabilities() const;

 private:
  WebMediaStreamSource::Capabilities platform_capabilities_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIASTREAM_INPUT_DEVICE_INFO_H_
