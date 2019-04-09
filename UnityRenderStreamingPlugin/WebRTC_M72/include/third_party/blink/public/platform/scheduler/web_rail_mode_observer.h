// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_SCHEDULER_WEB_RAIL_MODE_OBSERVER_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_SCHEDULER_WEB_RAIL_MODE_OBSERVER_H_

#include "third_party/blink/public/platform/web_common.h"
#include "v8/include/v8.h"

namespace blink {
namespace scheduler {

class BLINK_PLATFORM_EXPORT WebRAILModeObserver {
 public:
  virtual ~WebRAILModeObserver() = default;
  virtual void OnRAILModeChanged(v8::RAILMode rail_mode) = 0;
};

}  // namespace scheduler
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_SCHEDULER_WEB_RAIL_MODE_OBSERVER_H_
