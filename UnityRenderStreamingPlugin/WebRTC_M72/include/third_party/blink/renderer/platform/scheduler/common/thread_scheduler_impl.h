// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_COMMON_THREAD_SCHEDULER_IMPL_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_COMMON_THREAD_SCHEDULER_IMPL_H_

#include "third_party/blink/renderer/platform/platform_export.h"

#include <random>

#include "base/single_thread_task_runner.h"
#include "third_party/blink/public/platform/scheduler/web_thread_scheduler.h"
#include "third_party/blink/renderer/platform/scheduler/common/single_thread_idle_task_runner.h"
#include "third_party/blink/renderer/platform/scheduler/public/thread_scheduler.h"

namespace base {
class TickClock;

namespace sequence_manager {
class TimeDomain;
}
}  // namespace base

namespace blink {
namespace scheduler {
class SchedulerHelper;

// Scheduler-internal interface for the common methods between
// MainThreadSchedulerImpl and NonMainThreadSchedulerImpl which should
// not be exposed outside the scheduler.
class PLATFORM_EXPORT ThreadSchedulerImpl : public ThreadScheduler,
                                            public WebThreadScheduler {
 public:
  // This type is defined in both ThreadScheduler and WebThreadScheduler,
  // so the use of this type causes ambiguous lookup. Redefine this again
  // to hide the base classes' ones.
  using RendererPauseHandle = WebThreadScheduler::RendererPauseHandle;

  // Returns the idle task runner. Tasks posted to this runner may be reordered
  // relative to other task types and may be starved for an arbitrarily long
  // time if no idle time is available.
  virtual scoped_refptr<SingleThreadIdleTaskRunner> IdleTaskRunner() = 0;

  virtual scoped_refptr<base::SingleThreadTaskRunner> ControlTaskRunner() = 0;

  virtual void RegisterTimeDomain(
      base::sequence_manager::TimeDomain* time_domain) = 0;
  virtual void UnregisterTimeDomain(
      base::sequence_manager::TimeDomain* time_domain) = 0;
  virtual base::sequence_manager::TimeDomain* GetActiveTimeDomain() = 0;

  virtual const base::TickClock* GetTickClock() = 0;

 protected:
  ThreadSchedulerImpl();
  ~ThreadSchedulerImpl() override;

  // Returns true if the current task should not be reported in UKM because no
  // thread time was recorded for it. Also updates |sampling_rate| to account
  // for the ignored tasks by sampling the remaining tasks with higher
  // probability.
  bool ShouldIgnoreTaskForUkm(bool has_thread_time, double* sampling_rate);

  // Returns true with probability of kSamplingRateForTaskUkm.
  bool ShouldRecordTaskUkm(bool has_thread_time);

  virtual SchedulerHelper* GetHelper() = 0;

  void SetUkmTaskSamplingRateForTest(double sampling_rate);

  double ukm_task_sampling_rate_;

  std::mt19937_64 random_generator_;
  std::uniform_real_distribution<double> uniform_distribution_;
};

}  // namespace scheduler
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_COMMON_THREAD_SCHEDULER_IMPL_H_
