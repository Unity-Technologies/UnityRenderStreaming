// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_COMMON_DOWNLOAD_DOWNLOAD_STATS_H_
#define THIRD_PARTY_BLINK_PUBLIC_COMMON_DOWNLOAD_DOWNLOAD_STATS_H_

#include "base/macros.h"
#include "third_party/blink/public/common/common_export.h"

namespace blink {

class BLINK_COMMON_EXPORT DownloadStats {
 public:
  // These values are used to construct an enum in UMA. They should never be
  // changed.
  static constexpr unsigned kGestureBit = 0x1 << 0;
  static constexpr unsigned kAdBit = 0x1 << 1;
  static constexpr unsigned kCrossOriginBit = 0x1 << 2;
  static constexpr unsigned kSandboxBit = 0x1 << 3;
  static constexpr unsigned kCountSandboxOriginAdGesture = 16;

  static void RecordMainFrameHasGesture(bool gesture);
  static void RecordSubframeSandboxOriginAdGesture(unsigned value);

 private:
  DISALLOW_IMPLICIT_CONSTRUCTORS(DownloadStats);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_COMMON_DOWNLOAD_DOWNLOAD_STATS_H_
