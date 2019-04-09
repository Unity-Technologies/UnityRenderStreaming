// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// CancelableTaskTracker posts tasks (in the form of a Closure) to a
// TaskRunner, and is able to cancel the task later if it's not needed
// anymore.  On destruction, CancelableTaskTracker will cancel all
// tracked tasks.
//
// Each cancelable task can be associated with a reply (also a Closure). After
// the task is run on the TaskRunner, |reply| will be posted back to
// originating TaskRunner.
//
// NOTE:
//
// CancelableCallback (base/cancelable_callback.h) and WeakPtr binding are
// preferred solutions for canceling a task. However, they don't support
// cancelation from another sequence. This is sometimes a performance critical
// requirement. E.g. We need to cancel database lookup task on DB thread when
// user changes inputed text. If it is performance critical to do a best effort
// cancelation of a task, then CancelableTaskTracker is appropriate, otherwise
// use one of the other mechanisms.
//
// THREAD-SAFETY:
//
// 1. A CancelableTaskTracker object must be created, used, and destroyed on a
//    single sequence.
//
// 2. It's safe to destroy a CancelableTaskTracker while there are outstanding
//    tasks. This is commonly used to cancel all outstanding tasks.
//
// 3. The task is deleted on the target sequence, and the reply are deleted on
//    the originating sequence.
//
// 4. IsCanceledCallback can be run or deleted on any sequence.
#ifndef BASE_TASK_CANCELABLE_TASK_TRACKER_H_
#define BASE_TASK_CANCELABLE_TASK_TRACKER_H_

#include <stdint.h>

#include <memory>
#include <utility>

#include "base/base_export.h"
#include "base/bind.h"
#include "base/callback.h"
#include "base/containers/small_map.h"
#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "base/post_task_and_reply_with_result_internal.h"
#include "base/sequence_checker.h"

namespace base {

class CancellationFlag;
class Location;
class TaskRunner;

class BASE_EXPORT CancelableTaskTracker {
 public:
  // All values except kBadTaskId are valid.
  typedef int64_t TaskId;
  static const TaskId kBadTaskId;

  typedef Callback<bool()> IsCanceledCallback;

  CancelableTaskTracker();

  // Cancels all tracked tasks.
  ~CancelableTaskTracker();

  TaskId PostTask(TaskRunner* task_runner,
                  const Location& from_here,
                  OnceClosure task);

  TaskId PostTaskAndReply(TaskRunner* task_runner,
                          const Location& from_here,
                          OnceClosure task,
                          OnceClosure reply);

  template <typename TaskReturnType, typename ReplyArgType>
  TaskId PostTaskAndReplyWithResult(TaskRunner* task_runner,
                                    const Location& from_here,
                                    OnceCallback<TaskReturnType()> task,
                                    OnceCallback<void(ReplyArgType)> reply) {
    auto* result = new std::unique_ptr<TaskReturnType>();
    return PostTaskAndReply(
        task_runner, from_here,
        BindOnce(&internal::ReturnAsParamAdapter<TaskReturnType>,
                 std::move(task), Unretained(result)),
        BindOnce(&internal::ReplyAdapter<TaskReturnType, ReplyArgType>,
                 std::move(reply), Owned(result)));
  }

  // Callback version of PostTaskWithTraitsAndReplyWithResult above.
  // Though RepeatingCallback is convertible to OnceCallback, we need this since
  // we can not use template deduction and object conversion at once on the
  // overload resolution.
  // TODO(tzik): Update all callers of the Callback version to use OnceCallback.
  template <typename TaskReturnType, typename ReplyArgType>
  TaskId PostTaskAndReplyWithResult(TaskRunner* task_runner,
                                    const Location& from_here,
                                    Callback<TaskReturnType()> task,
                                    Callback<void(ReplyArgType)> reply) {
    return PostTaskAndReplyWithResult(
        task_runner, from_here,
        static_cast<OnceCallback<TaskReturnType()>>(std::move(task)),
        static_cast<OnceCallback<void(ReplyArgType)>>(std::move(reply)));
  }

  // Creates a tracked TaskId and an associated IsCanceledCallback. Client can
  // later call TryCancel() with the returned TaskId, and run |is_canceled_cb|
  // from any thread to check whether the TaskId is canceled.
  //
  // The returned task ID is tracked until the last copy of
  // |is_canceled_cb| is destroyed.
  //
  // Note. This function is used to address some special cancelation requirement
  // in existing code. You SHOULD NOT need this function in new code.
  TaskId NewTrackedTaskId(IsCanceledCallback* is_canceled_cb);

  // After calling this function, |task| and |reply| will not run. If the
  // cancelation happens when |task| is running or has finished running, |reply|
  // will not run. If |reply| is running or has finished running, cancellation
  // is a noop.
  //
  // Note. It's OK to cancel a |task| for more than once. The later calls are
  // noops.
  void TryCancel(TaskId id);

  // It's OK to call this function for more than once. The later calls are
  // noops.
  void TryCancelAll();

  // Returns true iff there are in-flight tasks that are still being
  // tracked.
  bool HasTrackedTasks() const;

 private:
  void Track(TaskId id, CancellationFlag* flag);
  void Untrack(TaskId id);

  // Typically the number of tasks are 0-2 and occationally 3-4. But since
  // this is a general API that could be used in unexpected ways, use a
  // small_map instead of a flat_map to avoid falling over if there are many
  // tasks.
  small_map<std::map<TaskId, CancellationFlag*>, 4> task_flags_;

  TaskId next_id_;
  SequenceChecker sequence_checker_;

  WeakPtrFactory<CancelableTaskTracker> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(CancelableTaskTracker);
};

}  // namespace base

#endif  // BASE_TASK_CANCELABLE_TASK_TRACKER_H_
