// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_THREADED_MULTI_THREADED_TEST_UTIL_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_THREADED_MULTI_THREADED_TEST_UTIL_H_

#include "testing/gtest/include/gtest/gtest.h"

#include <memory>

#include "base/single_thread_task_runner.h"
#include "third_party/blink/public/platform/platform.h"
#include "third_party/blink/renderer/platform/cross_thread_functional.h"
#include "third_party/blink/renderer/platform/scheduler/public/thread.h"
#include "third_party/blink/renderer/platform/scheduler/public/thread_scheduler.h"
#include "third_party/blink/renderer/platform/waitable_event.h"
#include "third_party/blink/renderer/platform/web_task_runner.h"
#include "third_party/blink/renderer/platform/web_thread_supporting_gc.h"
#include "third_party/blink/renderer/platform/wtf/functional.h"
#include "third_party/blink/renderer/platform/wtf/ref_counted.h"

namespace blink {

// This creates 2 macros for TSAN: TSAN_TEST and TSAN_TEST_F.
// Those tests are automatically disabled on non-tsan builds and should be used
// instead of the normal gtest macros for MultiThreadedTests.
// It guarantees that those tests are only run on Thread Sanitizer enabled
// builds.
// Also, TSAN_TEST subclasses MultiThreadTest instead of testing::Test.
#if defined(THREAD_SANITIZER)

#define TSAN_TEST(test_case_name, test_name)                         \
  GTEST_TEST_(test_case_name, test_name, ::blink::MultiThreadedTest, \
              testing::internal::GetTypeId<::blink::MultiThreadedTest>())

#define TSAN_TEST_F(test_fixture, test_name) TEST_F(test_fixture, test_name)

#else

#define TSAN_TEST(test_case_name, test_name)        \
  GTEST_TEST_(test_case_name, DISABLED_##test_name, \
              ::blink::MultiThreadedTest,           \
              testing::internal::GetTypeId<::blink::MultiThreadedTest>())

#define TSAN_TEST_F(test_fixture, test_name) \
  TEST_F(test_fixture, DISABLED_##test_name)

#endif

class MultiThreadedTest : public testing::Test {
 public:
  // RunOnThreads run a closure num_threads * callbacks_per_thread times.
  // The default for this is 10*100 = 1000 times.
  template <typename FunctionType, typename... Ps>
  void RunOnThreads(FunctionType function, Ps&&... parameters) {
    Vector<std::unique_ptr<WebThreadSupportingGC>> threads;
    Vector<std::unique_ptr<WaitableEvent>> waits;

    for (int i = 0; i < num_threads_; ++i) {
      threads.push_back(WebThreadSupportingGC::Create(
          ThreadCreationParams(WebThreadType::kTestThread)));
      waits.push_back(std::make_unique<WaitableEvent>());
    }

    for (int i = 0; i < num_threads_; ++i) {
      base::SingleThreadTaskRunner* task_runner =
          threads[i]->PlatformThread().GetTaskRunner().get();

      PostCrossThreadTask(*task_runner, FROM_HERE,
                          CrossThreadBind(
                              [](WebThreadSupportingGC* thread) {
                                thread->InitializeOnThread();
                              },
                              CrossThreadUnretained(threads[i].get())));

      for (int j = 0; j < callbacks_per_thread_; ++j) {
        PostCrossThreadTask(*task_runner, FROM_HERE,
                            CrossThreadBind(function, parameters...));
      }

      PostCrossThreadTask(
          *task_runner, FROM_HERE,
          CrossThreadBind(
              [](WebThreadSupportingGC* thread, WaitableEvent* w) {
                thread->ShutdownOnThread();
                w->Signal();
              },
              CrossThreadUnretained(threads[i].get()),
              CrossThreadUnretained(waits[i].get())));
    }

    for (int i = 0; i < num_threads_; ++i) {
      waits[i]->Wait();
    }
  }

 protected:
  int num_threads_ = 10;
  int callbacks_per_thread_ = 100;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_THREADED_MULTI_THREADED_TEST_UTIL_H_
