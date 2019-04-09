// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_COMMON_PICTURE_IN_PICTURE_PICTURE_IN_PICTURE_CONTROL_INFO_H_
#define THIRD_PARTY_BLINK_PUBLIC_COMMON_PICTURE_IN_PICTURE_PICTURE_IN_PICTURE_CONTROL_INFO_H_

#include <string>
#include <vector>
#include "ui/gfx/geometry/size.h"
#include "url/gurl.h"

namespace blink {

// PictureInPictureControlInfo passes information about the desired custom
// controls for a Picture-in-Picture window from the Web API to the
// OverlayWindow.
struct PictureInPictureControlInfo {
  // These vectors represent the members of a deserialized MediaImage.
  struct Icon {
    GURL src;
    std::vector<gfx::Size> sizes;
    std::string type;
  };

  // |id| is the unique name of the custom control.
  std::string id;

  // |label| is the descriptive text used for accessibility and hover text.
  std::string label;

  // |icons| contains the images used for the custom control.
  std::vector<Icon> icons;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_COMMON_PICTURE_IN_PICTURE_PICTURE_IN_PICTURE_CONTROL_INFO_H_
