// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TASK_LAZY_TASK_RUNNER_H_
#define BASE_TASK_LAZY_TASK_RUNNER_H_

#include <vector>

#include "base/atomicops.h"
#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/lazy_instance_helpers.h"
#include "base/sequenced_task_runner.h"
#include "base/single_thread_task_runner.h"
#include "base/task/single_thread_task_runner_thread_mode.h"
#include "base/task/task_scheduler/scheduler_lock.h"
#include "base/task/task_traits.h"
#include "build/build_config.h"

// Lazy(Sequenced|SingleThread|COMSTA)TaskRunner lazily creates a TaskRunner.
//
// Lazy(Sequenced|SingleThread|COMSTA)TaskRunner is meant to be instantiated in
// an anonymous namespace (no static initializer is generated) and used to post
// tasks to the same sequence/thread from pieces of code that don't have a
// better way of sharing a TaskRunner. It is important to use this class
// instead of a self-managed global variable or LazyInstance so that the
// TaskRunners do not outlive the scope of the ScopedTaskEnvironment in unit
// tests (otherwise the next test in the same process will die in use-after-
// frees).
//
// IMPORTANT: Only use this API as a last resort. Prefer storing a
// (Sequenced|SingleThread)TaskRunner returned by
// base::Create(Sequenced|SingleThread|COMSTA)TaskRunnerWithTraits() as a member
// on an object accessible by all PostTask() call sites.
//
// Example usage 1:
//
// namespace {
// base::LazySequencedTaskRunner g_sequenced_task_runner =
//     LAZY_SEQUENCED_TASK_RUNNER_INITIALIZER(
//         base::TaskTraits(base::MayBlock(),
//                          base::TaskPriority::USER_VISIBLE));
// }  // namespace
//
// void SequencedFunction() {
//   // Different invocations of this function post to the same
//   // MayBlock() SequencedTaskRunner.
//   g_sequenced_task_runner.Get()->PostTask(FROM_HERE, base::BindOnce(...));
// }
//
// Example usage 2:
//
// namespace {
// base::LazySequencedTaskRunner g_sequenced_task_task_runner =
//     LAZY_SEQUENCED_TASK_RUNNER_INITIALIZER({base::MayBlock()});
// }  // namespace
//
// // Code from different files can access the SequencedTaskRunner via this
// // function.
// scoped_refptr<base::SequencedTaskRunner> GetTaskRunner() {
//   return g_sequenced_task_runner.Get();
// }

namespace base {

namespace internal {
template <typename TaskRunnerType, bool com_sta>
class BASE_EXPORT LazyTaskRunner;
}  // namespace internal

// Lazy SequencedTaskRunner.
using LazySequencedTaskRunner =
    internal::LazyTaskRunner<SequencedTaskRunner, false>;

// Lazy SingleThreadTaskRunner.
using LazySingleThreadTaskRunner =
    internal::LazyTaskRunner<SingleThreadTaskRunner, false>;

#if defined(OS_WIN)
// Lazy COM-STA enabled SingleThreadTaskRunner.
using LazyCOMSTATaskRunner =
    internal::LazyTaskRunner<SingleThreadTaskRunner, true>;
#endif

// Helper macros to generate a variable name by concatenation.
#define LAZY_TASK_RUNNER_CONCATENATE_INTERNAL2(a, b) a##b
#define LAZY_TASK_RUNNER_CONCATENATE_INTERNAL(a, b) \
  LAZY_TASK_RUNNER_CONCATENATE_INTERNAL2(a, b)

// Use the macros below to initialize a LazyTaskRunner. These macros verify that
// their arguments are constexpr, which is important to prevent the generation
// of a static initializer.

// |traits| are TaskTraits used when creating the SequencedTaskRunner.
#define LAZY_SEQUENCED_TASK_RUNNER_INITIALIZER(traits)                 \
  base::LazySequencedTaskRunner::CreateInternal(traits);               \
  ALLOW_UNUSED_TYPE constexpr base::TaskTraits                         \
      LAZY_TASK_RUNNER_CONCATENATE_INTERNAL(kVerifyTraitsAreConstexpr, \
                                            __LINE__) = traits

// |traits| are TaskTraits used when creating the SingleThreadTaskRunner.
// |thread_mode| specifies whether the SingleThreadTaskRunner can share its
// thread with other SingleThreadTaskRunners.
#define LAZY_SINGLE_THREAD_TASK_RUNNER_INITIALIZER(traits, thread_mode)   \
  base::LazySingleThreadTaskRunner::CreateInternal(traits, thread_mode);  \
  ALLOW_UNUSED_TYPE constexpr base::TaskTraits                            \
      LAZY_TASK_RUNNER_CONCATENATE_INTERNAL(kVerifyTraitsAreConstexpr,    \
                                            __LINE__) = traits;           \
  ALLOW_UNUSED_TYPE constexpr base::SingleThreadTaskRunnerThreadMode      \
      LAZY_TASK_RUNNER_CONCATENATE_INTERNAL(kVerifyThreadModeIsConstexpr, \
                                            __LINE__) = thread_mode

// |traits| are TaskTraits used when creating the COM STA
// SingleThreadTaskRunner. |thread_mode| specifies whether the COM STA
// SingleThreadTaskRunner can share its thread with other
// SingleThreadTaskRunners.
#define LAZY_COM_STA_TASK_RUNNER_INITIALIZER(traits, thread_mode)         \
  base::LazyCOMSTATaskRunner::CreateInternal(traits, thread_mode);        \
  ALLOW_UNUSED_TYPE constexpr base::TaskTraits                            \
      LAZY_TASK_RUNNER_CONCATENATE_INTERNAL(kVerifyTraitsAreConstexpr,    \
                                            __LINE__) = traits;           \
  ALLOW_UNUSED_TYPE constexpr base::SingleThreadTaskRunnerThreadMode      \
      LAZY_TASK_RUNNER_CONCATENATE_INTERNAL(kVerifyThreadModeIsConstexpr, \
                                            __LINE__) = thread_mode

namespace internal {

template <typename TaskRunnerType, bool com_sta>
class BASE_EXPORT LazyTaskRunner {
 public:
  // Use the macros above rather than a direct call to this.
  //
  // |traits| are TaskTraits to use to create the TaskRunner. If this
  // LazyTaskRunner is specialized to create a SingleThreadTaskRunner,
  // |thread_mode| specifies whether the SingleThreadTaskRunner can share its
  // thread with other SingleThreadTaskRunner. Otherwise, it is unused.
  static constexpr LazyTaskRunner CreateInternal(
      const TaskTraits& traits,
      SingleThreadTaskRunnerThreadMode thread_mode =
          SingleThreadTaskRunnerThreadMode::SHARED) {
    return LazyTaskRunner(traits, thread_mode);
  }

  // Returns the TaskRunner held by this instance. Creates it if it didn't
  // already exist. Thread-safe.
  scoped_refptr<TaskRunnerType> Get();

 private:
  constexpr LazyTaskRunner(const TaskTraits& traits,
                           SingleThreadTaskRunnerThreadMode thread_mode =
                               SingleThreadTaskRunnerThreadMode::SHARED)
      : traits_(traits), thread_mode_(thread_mode) {}

  // Releases the TaskRunner held by this instance.
  void Reset();

  // Creates and returns a new TaskRunner.
  scoped_refptr<TaskRunnerType> Create();

  // Creates a new TaskRunner via Create(), adds an explicit ref to it, and
  // returns it raw. Used as an adapter for lazy instance helpers. Static and
  // takes |this| as an explicit param to match the void* signature of
  // GetOrCreateLazyPointer().
  static TaskRunnerType* CreateRaw(void* void_self);

  // TaskTraits to create the TaskRunner.
  const TaskTraits traits_;

  // SingleThreadTaskRunnerThreadMode to create the TaskRunner.
  const SingleThreadTaskRunnerThreadMode thread_mode_;

  // Can have 3 states:
  // - This instance does not hold a TaskRunner: 0
  // - This instance is creating a TaskRunner: kLazyInstanceStateCreating
  // - This instance holds a TaskRunner: Pointer to the TaskRunner.
  // LazyInstance's internals are reused to handle transition between states.
  subtle::AtomicWord state_ = 0;

  // No DISALLOW_COPY_AND_ASSIGN since that prevents static initialization with
  // Visual Studio (warning C4592: 'symbol will be dynamically initialized
  // (implementation limitation))'.
};

// When a LazyTaskRunner becomes active (invokes Get()), it adds a callback to
// the current ScopedLazyTaskRunnerListForTesting, if any. Callbacks run when
// the ScopedLazyTaskRunnerListForTesting is destroyed. In a test process, a
// ScopedLazyTaskRunnerListForTesting must be instantiated before any
// LazyTaskRunner becomes active.
class BASE_EXPORT ScopedLazyTaskRunnerListForTesting {
 public:
  ScopedLazyTaskRunnerListForTesting();
  ~ScopedLazyTaskRunnerListForTesting();

 private:
  friend class LazyTaskRunner<SequencedTaskRunner, false>;
  friend class LazyTaskRunner<SingleThreadTaskRunner, false>;

#if defined(OS_WIN)
  friend class LazyTaskRunner<SingleThreadTaskRunner, true>;
#endif

  // Add |callback| to the list of callbacks to run on destruction.
  void AddCallback(OnceClosure callback);

  // Synchronizes accesses to |callbacks_|.
  SchedulerLock lock_;

  // List of callbacks to run on destruction.
  std::vector<OnceClosure> callbacks_;

  DISALLOW_COPY_AND_ASSIGN(ScopedLazyTaskRunnerListForTesting);
};

}  // namespace internal
}  // namespace base

#endif  // BASE_TASK_LAZY_TASK_RUNNER_H_
