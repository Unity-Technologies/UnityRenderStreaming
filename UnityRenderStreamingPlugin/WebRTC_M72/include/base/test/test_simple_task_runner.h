// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TEST_TEST_SIMPLE_TASK_RUNNER_H_
#define BASE_TEST_TEST_SIMPLE_TASK_RUNNER_H_

#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/containers/circular_deque.h"
#include "base/macros.h"
#include "base/single_thread_task_runner.h"
#include "base/synchronization/lock.h"
#include "base/test/test_pending_task.h"
#include "base/threading/platform_thread.h"

namespace base {

class TimeDelta;

// TestSimpleTaskRunner is a simple TaskRunner implementation that can
// be used for testing.  It implements SingleThreadTaskRunner as that
// interface implements SequencedTaskRunner, which in turn implements
// TaskRunner, so TestSimpleTaskRunner can be passed in to a function
// that accepts any *TaskRunner object.
//
// TestSimpleTaskRunner has the following properties which make it simple:
//
//   - Tasks are simply stored in a queue in FIFO order, ignoring delay
//     and nestability.
//   - Tasks aren't guaranteed to be destroyed immediately after
//     they're run.
//
// However, TestSimpleTaskRunner allows for reentrancy, in that it
// handles the running of tasks that in turn call back into itself
// (e.g., to post more tasks).
//
// Note that, like any TaskRunner, TestSimpleTaskRunner is
// ref-counted.
class TestSimpleTaskRunner : public SingleThreadTaskRunner {
 public:
  TestSimpleTaskRunner();

  // SingleThreadTaskRunner implementation.
  bool PostDelayedTask(const Location& from_here,
                       OnceClosure task,
                       TimeDelta delay) override;
  bool PostNonNestableDelayedTask(const Location& from_here,
                                  OnceClosure task,
                                  TimeDelta delay) override;

  bool RunsTasksInCurrentSequence() const override;

  base::circular_deque<TestPendingTask> TakePendingTasks();
  size_t NumPendingTasks() const;
  bool HasPendingTask() const;
  base::TimeDelta NextPendingTaskDelay() const;
  base::TimeDelta FinalPendingTaskDelay() const;

  // Clears the queue of pending tasks without running them.
  void ClearPendingTasks();

  // Runs each current pending task in order and clears the queue. Tasks posted
  // by the tasks that run within this call do not run within this call. Can
  // only be called on the thread that created this TestSimpleTaskRunner.
  void RunPendingTasks();

  // Runs pending tasks until the queue is empty. Can only be called on the
  // thread that created this TestSimpleTaskRunner.
  void RunUntilIdle();

 protected:
  ~TestSimpleTaskRunner() override;

 private:
  // Thread on which this was instantiated.
  const PlatformThreadRef thread_ref_ = PlatformThread::CurrentRef();

  // Synchronizes access to |pending_tasks_|.
  mutable Lock lock_;

  base::circular_deque<TestPendingTask> pending_tasks_;

  DISALLOW_COPY_AND_ASSIGN(TestSimpleTaskRunner);
};

}  // namespace base

#endif  // BASE_TEST_TEST_SIMPLE_TASK_RUNNER_H_
