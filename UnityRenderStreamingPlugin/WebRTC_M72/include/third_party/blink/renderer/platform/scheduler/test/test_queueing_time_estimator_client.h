// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_TEST_TEST_QUEUEING_TIME_ESTIMATOR_CLIENT_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_TEST_TEST_QUEUEING_TIME_ESTIMATOR_CLIENT_H_

#include "third_party/blink/renderer/platform/scheduler/main_thread/queueing_time_estimator.h"

#include <map>
#include <vector>

namespace blink {
namespace scheduler {

using QueueType = MainThreadTaskQueue::QueueType;

class TestQueueingTimeEstimatorClient : public QueueingTimeEstimator::Client {
 public:
  // QueueingTimeEstimator::Client implementation:
  void OnQueueingTimeForWindowEstimated(base::TimeDelta queueing_time,
                                        bool is_disjoint_window) override;
  void OnReportFineGrainedExpectedQueueingTime(
      const char* split_description,
      base::TimeDelta queueing_time) override;

  const std::vector<base::TimeDelta>& expected_queueing_times() {
    return expected_queueing_times_;
  }
  const std::map<std::string, std::vector<base::TimeDelta>>& split_eqts() {
    return split_eqts_;
  }
  const std::vector<base::TimeDelta>& QueueTypeValues(QueueType queue_type);
  const std::vector<base::TimeDelta>& FrameStatusValues(
      FrameStatus frame_status);

 private:
  std::vector<base::TimeDelta> expected_queueing_times_;
  std::map<std::string, std::vector<base::TimeDelta>> split_eqts_;
};

class QueueingTimeEstimatorForTest : public QueueingTimeEstimator {
 public:
  QueueingTimeEstimatorForTest(TestQueueingTimeEstimatorClient* client,
                               base::TimeDelta window_duration,
                               int steps_per_window,
                               base::TimeTicks time);
};

}  // namespace scheduler
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_TEST_TEST_QUEUEING_TIME_ESTIMATOR_CLIENT_H_
