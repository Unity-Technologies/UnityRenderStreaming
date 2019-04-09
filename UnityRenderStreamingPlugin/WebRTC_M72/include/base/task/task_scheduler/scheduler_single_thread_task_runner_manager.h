// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TASK_TASK_SCHEDULER_SCHEDULER_SINGLE_THREAD_TASK_RUNNER_MANAGER_H_
#define BASE_TASK_TASK_SCHEDULER_SCHEDULER_SINGLE_THREAD_TASK_RUNNER_MANAGER_H_

#include <memory>
#include <string>
#include <vector>

#include "base/base_export.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/task/single_thread_task_runner_thread_mode.h"
#include "base/task/task_scheduler/environment_config.h"
#include "base/task/task_scheduler/scheduler_lock.h"
#include "base/task/task_scheduler/tracked_ref.h"
#include "base/threading/platform_thread.h"
#include "build/build_config.h"

namespace base {

class TaskTraits;
class SchedulerWorkerObserver;
class SingleThreadTaskRunner;

namespace internal {

class DelayedTaskManager;
class SchedulerWorker;
class TaskTracker;

namespace {

class SchedulerWorkerDelegate;

}  // namespace

// Manages a pool of threads which are each associated with one or more
// SingleThreadTaskRunners.
//
// SingleThreadTaskRunners using SingleThreadTaskRunnerThreadMode::SHARED are
// backed by shared SchedulerWorkers for each COM+task environment combination.
// These workers are lazily instantiated and then only reclaimed during
// JoinForTesting()
//
// No threads are created (and hence no tasks can run) before Start() is called.
//
// This class is thread-safe.
class BASE_EXPORT SchedulerSingleThreadTaskRunnerManager final {
 public:
  SchedulerSingleThreadTaskRunnerManager(
      TrackedRef<TaskTracker> task_tracker,
      DelayedTaskManager* delayed_task_manager);
  ~SchedulerSingleThreadTaskRunnerManager();

  // Starts threads for existing SingleThreadTaskRunners and allows threads to
  // be started when SingleThreadTaskRunners are created in the future. If
  // specified, |scheduler_worker_observer| will be notified when a worker
  // enters and exits its main function. It must not be destroyed before
  // JoinForTesting() has returned (must never be destroyed in production).
  void Start(SchedulerWorkerObserver* scheduler_worker_observer = nullptr);

  // Creates a SingleThreadTaskRunner which runs tasks with |traits| on a thread
  // named "TaskSchedulerSingleThread[Shared]" +
  // kEnvironmentParams[GetEnvironmentIndexForTraits(traits)].name_suffix +
  // index.
  scoped_refptr<SingleThreadTaskRunner> CreateSingleThreadTaskRunnerWithTraits(
      const TaskTraits& traits,
      SingleThreadTaskRunnerThreadMode thread_mode);

#if defined(OS_WIN)
  // Creates a SingleThreadTaskRunner which runs tasks with |traits| on a COM
  // STA thread named "TaskSchedulerSingleThreadCOMSTA[Shared]" +
  // kEnvironmentParams[GetEnvironmentIndexForTraits(traits)].name_suffix +
  // index.
  scoped_refptr<SingleThreadTaskRunner> CreateCOMSTATaskRunnerWithTraits(
      const TaskTraits& traits,
      SingleThreadTaskRunnerThreadMode thread_mode);
#endif  // defined(OS_WIN)

  void JoinForTesting();

 private:
  class SchedulerSingleThreadTaskRunner;

  enum ContinueOnShutdown {
    IS_CONTINUE_ON_SHUTDOWN,
    IS_NOT_CONTINUE_ON_SHUTDOWN,
    CONTINUE_ON_SHUTDOWN_COUNT,
  };

  static ContinueOnShutdown TraitsToContinueOnShutdown(
      const TaskTraits& traits);

  template <typename DelegateType>
  scoped_refptr<SchedulerSingleThreadTaskRunner> CreateTaskRunnerWithTraitsImpl(
      const TaskTraits& traits,
      SingleThreadTaskRunnerThreadMode thread_mode);

  template <typename DelegateType>
  std::unique_ptr<SchedulerWorkerDelegate> CreateSchedulerWorkerDelegate(
      const std::string& name,
      int id,
      SingleThreadTaskRunnerThreadMode thread_mode);

  template <typename DelegateType>
  SchedulerWorker* CreateAndRegisterSchedulerWorker(
      const std::string& name,
      SingleThreadTaskRunnerThreadMode thread_mode,
      ThreadPriority priority_hint);

  template <typename DelegateType>
  SchedulerWorker*& GetSharedSchedulerWorkerForTraits(const TaskTraits& traits);

  void UnregisterSchedulerWorker(SchedulerWorker* worker);

  void ReleaseSharedSchedulerWorkers();

  const TrackedRef<TaskTracker> task_tracker_;
  DelayedTaskManager* const delayed_task_manager_;

  // Optional observer notified when a worker enters and exits its main
  // function. Set in Start() and never modified afterwards.
  SchedulerWorkerObserver* scheduler_worker_observer_ = nullptr;

  // Synchronizes access to all members below.
  SchedulerLock lock_;
  std::vector<scoped_refptr<SchedulerWorker>> workers_;
  int next_worker_id_ = 0;

  // Workers for SingleThreadTaskRunnerThreadMode::SHARED tasks. It is
  // important to have separate threads for CONTINUE_ON_SHUTDOWN and non-
  // CONTINUE_ON_SHUTDOWN to avoid being in a situation where a
  // CONTINUE_ON_SHUTDOWN task effectively blocks shutdown by preventing a
  // BLOCK_SHUTDOWN task to be scheduled. https://crbug.com/829786
  SchedulerWorker* shared_scheduler_workers_[ENVIRONMENT_COUNT]
                                            [CONTINUE_ON_SHUTDOWN_COUNT] = {};
#if defined(OS_WIN)
  SchedulerWorker* shared_com_scheduler_workers_[ENVIRONMENT_COUNT]
                                                [CONTINUE_ON_SHUTDOWN_COUNT] =
                                                    {};
#endif  // defined(OS_WIN)

  // Set to true when Start() is called.
  bool started_ = false;

  DISALLOW_COPY_AND_ASSIGN(SchedulerSingleThreadTaskRunnerManager);
};

}  // namespace internal
}  // namespace base

#endif  // BASE_TASK_TASK_SCHEDULER_SCHEDULER_SINGLE_THREAD_TASK_RUNNER_MANAGER_H_
