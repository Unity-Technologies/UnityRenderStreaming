// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_TEST_FUZZER_TASK_QUEUE_WITH_VOTERS_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_TEST_FUZZER_TASK_QUEUE_WITH_VOTERS_H_

#include <vector>

#include "base/memory/scoped_refptr.h"
#include "base/task/sequence_manager/test/test_task_queue.h"
#include "third_party/blink/renderer/platform/platform_export.h"

namespace base {
namespace sequence_manager {

struct PLATFORM_EXPORT TaskQueueWithVoters {
  explicit TaskQueueWithVoters(scoped_refptr<TestTaskQueue> task_queue)
      : queue(std::move(task_queue)){};

  scoped_refptr<TestTaskQueue> queue;
  std::vector<std::unique_ptr<TaskQueue::QueueEnabledVoter>> voters;
};

}  // namespace sequence_manager
}  // namespace base

#endif
