// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_PUBLIC_WORKER_SCHEDULER_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_PUBLIC_WORKER_SCHEDULER_H_

#include "base/memory/weak_ptr.h"
#include "base/single_thread_task_runner.h"
#include "third_party/blink/public/platform/task_type.h"
#include "third_party/blink/renderer/platform/scheduler/public/frame_or_worker_scheduler.h"
#include "third_party/blink/renderer/platform/scheduler/public/frame_scheduler.h"

namespace blink {
namespace scheduler {

class NonMainThreadTaskQueue;
class WorkerSchedulerProxy;
class WorkerThreadScheduler;

// A scheduler provides per-global-scope task queues. This is constructed when a
// global scope is created and destructed when it's closed.
//
// Unless stated otherwise, all methods must be called on the worker thread.
class PLATFORM_EXPORT WorkerScheduler : public FrameOrWorkerScheduler {
 public:
  WorkerScheduler(WorkerThreadScheduler* worker_thread_scheduler,
                  WorkerSchedulerProxy* proxy);
  ~WorkerScheduler() override;

  std::unique_ptr<ActiveConnectionHandle> OnActiveConnectionCreated() override;

  // Unregisters the task queues and cancels tasks in them.
  void Dispose();

  // Returns a task runner that is suitable with the given task type. This can
  // be called from any thread.
  //
  // This must be called only from WorkerThread::GetTaskRunner().
  scoped_refptr<base::SingleThreadTaskRunner> GetTaskRunner(TaskType) const;

  WorkerThreadScheduler* GetWorkerThreadScheduler() const {
    return thread_scheduler_;
  }

  void OnLifecycleStateChanged(SchedulingLifecycleState lifecycle_state);

  SchedulingLifecycleState CalculateLifecycleState(ObserverType) const override;

 protected:
  scoped_refptr<NonMainThreadTaskQueue> ThrottleableTaskQueue();
  scoped_refptr<NonMainThreadTaskQueue> UnthrottleableTaskQueue();

 private:
  void SetUpThrottling();

  base::WeakPtr<WorkerScheduler> GetWeakPtr();

  scoped_refptr<NonMainThreadTaskQueue> throttleable_task_queue_;
  scoped_refptr<NonMainThreadTaskQueue> unthrottleable_task_queue_;

  SchedulingLifecycleState lifecycle_state_ =
      SchedulingLifecycleState::kNotThrottled;

  WorkerThreadScheduler* thread_scheduler_;  // NOT OWNED

#if DCHECK_IS_ON()
  bool is_disposed_ = false;
#endif

  base::WeakPtrFactory<WorkerScheduler> weak_factory_;
};

}  // namespace scheduler
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_PUBLIC_WORKER_SCHEDULER_H_
