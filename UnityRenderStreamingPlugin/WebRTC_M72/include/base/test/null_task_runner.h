// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TEST_NULL_TASK_RUNNER_H_
#define BASE_TEST_NULL_TASK_RUNNER_H_

#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/macros.h"
#include "base/single_thread_task_runner.h"

namespace base {

// Helper class for tests that need to provide an implementation of a
// *TaskRunner class but don't actually care about tasks being run.

class NullTaskRunner : public base::SingleThreadTaskRunner {
 public:
  NullTaskRunner();

  bool PostDelayedTask(const Location& from_here,
                       base::OnceClosure task,
                       base::TimeDelta delay) override;
  bool PostNonNestableDelayedTask(const Location& from_here,
                                  base::OnceClosure task,
                                  base::TimeDelta delay) override;
  // Always returns true to avoid triggering DCHECKs.
  bool RunsTasksInCurrentSequence() const override;

 protected:
  ~NullTaskRunner() override;

  DISALLOW_COPY_AND_ASSIGN(NullTaskRunner);
};

}  // namespace base

#endif  // BASE_TEST_NULL_TASK_RUNNER_H_
