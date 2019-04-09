// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_PUBLIC_THREAD_CPU_THROTTLER_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_PUBLIC_THREAD_CPU_THROTTLER_H_

#include <memory>

#include "base/macros.h"
#include "third_party/blink/renderer/platform/platform_export.h"

namespace base {
template <typename T>
struct DefaultSingletonTraits;
};

namespace blink {
namespace scheduler {

// This class is used to slow down the main thread for
// inspector "cpu throttling". It does it by spawning an
// additional thread which frequently interrupts main thread
// and sleeps.
class PLATFORM_EXPORT ThreadCPUThrottler final {
 public:
  static ThreadCPUThrottler* GetInstance();

  // |rate| is a slow-down factor - passing 2.0 will make
  // everything two times slower.
  // Any rate less or equal to 1.0 disables throttling and
  // cleans up helper thread.
  void SetThrottlingRate(double rate);

 private:
  ThreadCPUThrottler();
  ~ThreadCPUThrottler();
  friend struct base::DefaultSingletonTraits<ThreadCPUThrottler>;

  class ThrottlingThread;
  std::unique_ptr<ThrottlingThread> throttling_thread_;

  DISALLOW_COPY_AND_ASSIGN(ThreadCPUThrottler);
};

}  // namespace scheduler
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_PUBLIC_THREAD_CPU_THROTTLER_H_
