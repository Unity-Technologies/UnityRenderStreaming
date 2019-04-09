// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TASK_TASK_SCHEDULER_SCHEDULER_LOCK_IMPL_H_
#define BASE_TASK_TASK_SCHEDULER_SCHEDULER_LOCK_IMPL_H_

#include <memory>

#include "base/base_export.h"
#include "base/macros.h"
#include "base/synchronization/lock.h"

namespace base {

class ConditionVariable;

namespace internal {

struct UniversalPredecessor {};

// A regular lock with simple deadlock correctness checking.
// This lock tracks all of the available locks to make sure that any locks are
// acquired in an expected order.
// See scheduler_lock.h for details.
class BASE_EXPORT SchedulerLockImpl {
 public:
  SchedulerLockImpl();
  explicit SchedulerLockImpl(const SchedulerLockImpl* predecessor);
  explicit SchedulerLockImpl(UniversalPredecessor);
  ~SchedulerLockImpl();

  void Acquire();
  void Release();

  void AssertAcquired() const;

  std::unique_ptr<ConditionVariable> CreateConditionVariable();

  bool is_universal_predecessor() const { return is_universal_predecessor_; }

 private:
  Lock lock_;
  const bool is_universal_predecessor_;

  DISALLOW_COPY_AND_ASSIGN(SchedulerLockImpl);
};

}  // namespace internal
}  // namespace base

#endif  // BASE_TASK_TASK_SCHEDULER_SCHEDULER_LOCK_IMPL_H_
