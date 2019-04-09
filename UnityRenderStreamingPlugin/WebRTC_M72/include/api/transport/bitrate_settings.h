/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_TRANSPORT_BITRATE_SETTINGS_H_
#define API_TRANSPORT_BITRATE_SETTINGS_H_

#include "absl/types/optional.h"

namespace webrtc {

// Configuration of send bitrate. The |start_bitrate_bps| value is
// used for multiple purposes, both as a prior in the bandwidth
// estimator, and for initial configuration of the encoder. We may
// want to create separate apis for those, and use a smaller struct
// with only the min and max constraints.
struct BitrateSettings {
  BitrateSettings();
  ~BitrateSettings();
  BitrateSettings(const BitrateSettings&);
  // 0 <= min <= start <= max should hold for set parameters.
  absl::optional<int> min_bitrate_bps;
  absl::optional<int> start_bitrate_bps;
  absl::optional<int> max_bitrate_bps;
};

}  // namespace webrtc

#endif  // API_TRANSPORT_BITRATE_SETTINGS_H_
