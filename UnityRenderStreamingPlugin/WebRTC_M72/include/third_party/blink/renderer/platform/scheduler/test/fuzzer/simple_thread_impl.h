// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_TEST_FUZZER_SIMPLE_THREAD_IMPL_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_TEST_FUZZER_SIMPLE_THREAD_IMPL_H_

#include "base/callback.h"
#include "base/synchronization/waitable_event.h"
#include "base/threading/simple_thread.h"
#include "third_party/blink/renderer/platform/platform_export.h"

namespace base {
namespace sequence_manager {

class ThreadPoolManager;
class ThreadManager;

// Used by the ThreadPoolManager to create threads that do not have an
// associated message loop, since we want to use base::TestMockTimeTaskRunner to
// control the task execution and the clock of the thread.
class PLATFORM_EXPORT SimpleThreadImpl : public SimpleThread {
 public:
  using ThreadCallback = base::OnceCallback<void(ThreadManager*)>;

  SimpleThreadImpl(ThreadPoolManager* thread_pool_manager,
                   TimeTicks initial_time,
                   ThreadCallback callback);

  ~SimpleThreadImpl() override;

  ThreadManager* thread_manager() const;

 private:
  // This doesn't terminate until |this| object is destructed.
  void Run() override;

  // Owner of this class.
  ThreadPoolManager* thread_pool_manager_ = nullptr;

  // Time in which the thread is created.
  TimeTicks initial_time_;

  // The object pointed to by |thread_manager_| is created and destructed from
  // the Run function. This is necessary since it has to be constructed from the
  // thread it should be bound to and destructed from the same thread.
  ThreadManager* thread_manager_ = nullptr;

  // Used by the Run function to only terminate when |this| is destructed, and
  // this is used so that |thread_data_| will live as long as |this|.
  WaitableEvent thread_can_shutdown_;

  ThreadCallback callback_;
};

}  // namespace sequence_manager
}  // namespace base

#endif
