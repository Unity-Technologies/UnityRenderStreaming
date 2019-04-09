// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_COMMON_MANIFEST_MANIFEST_ICON_SELECTOR_H_
#define THIRD_PARTY_BLINK_PUBLIC_COMMON_MANIFEST_MANIFEST_ICON_SELECTOR_H_

#include "base/macros.h"
#include "third_party/blink/public/common/manifest/manifest.h"
#include "url/gurl.h"

namespace blink {

// Selects the square icon with the supported image MIME types and the specified
// icon purpose that most closely matches the size constraints.
// This follows very basic heuristics -- improvements are welcome.
class BLINK_COMMON_EXPORT ManifestIconSelector {
 public:
  // Runs the algorithm to find the best matching icon in the icons listed in
  // the Manifest. Size is defined in pixels.
  //
  // Any icon returned will be close as possible to |ideal_icon_size_in_px|
  // with a size not less than |minimum_icon_size_in_px|. Additionally, it must
  // be square, have supported image MIME types, and have icon purpose
  // |purpose|.
  //
  // Returns the icon url if a suitable icon is found. An empty URL otherwise.
  static GURL FindBestMatchingIcon(
      const std::vector<blink::Manifest::ImageResource>& icons,
      int ideal_icon_size_in_px,
      int minimum_icon_size_in_px,
      blink::Manifest::ImageResource::Purpose purpose);

 private:
  DISALLOW_IMPLICIT_CONSTRUCTORS(ManifestIconSelector);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_COMMON_MANIFEST_MANIFEST_ICON_SELECTOR_H_
