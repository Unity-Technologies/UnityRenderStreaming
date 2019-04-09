// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_COMMON_PRIVACY_PREFERENCES_H_
#define THIRD_PARTY_BLINK_PUBLIC_COMMON_PRIVACY_PREFERENCES_H_

#include "third_party/blink/public/common/common_export.h"

namespace blink {

// Subset of content::RendererPreferences for passing the security info to
// blink.
// TODO(crbug.com/869748): Move this into a mojom struct and use the new struct
// as a part of RendererPreferences once RendererPreferences becomes a mojom
// struct.
struct BLINK_COMMON_EXPORT PrivacyPreferences {
  PrivacyPreferences() = default;
  PrivacyPreferences(bool enable_do_not_track,
                     bool enable_referrers);

  // These default values are coming from the defaults in
  // content::RendererPreferences.
  bool enable_do_not_track = false;
  bool enable_referrers = true;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_COMMON_PRIVACY_PREFERENCES_H_
