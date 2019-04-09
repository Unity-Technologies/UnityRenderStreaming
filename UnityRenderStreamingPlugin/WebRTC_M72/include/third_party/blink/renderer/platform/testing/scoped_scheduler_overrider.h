// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_SCOPED_SCHEDULER_OVERRIDER_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_SCOPED_SCHEDULER_OVERRIDER_H_

#include "third_party/blink/renderer/platform/scheduler/public/thread_scheduler.h"
#include "third_party/blink/renderer/platform/testing/scoped_main_thread_overrider.h"

namespace blink {

// A utility which lets you inject your custom implementation of ThreadScheduler
// on the main thread. This class sets up a very simple instance of
// blink::Thread and overrides the global main thread using ScopedMainThread-
// Overrider. Multi-thread is not supported.

class ScopedSchedulerOverrider final {
 public:
  // |scheduler| must be owned by the caller.
  explicit ScopedSchedulerOverrider(ThreadScheduler* scheduler);
  ~ScopedSchedulerOverrider();

 private:
  ScopedMainThreadOverrider main_thread_overrider_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_SCOPED_SCHEDULER_OVERRIDER_H_
