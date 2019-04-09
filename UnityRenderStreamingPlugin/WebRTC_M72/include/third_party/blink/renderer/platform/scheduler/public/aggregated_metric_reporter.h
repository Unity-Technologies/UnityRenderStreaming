// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_PUBLIC_AGGREGATED_METRIC_REPORTER_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_PUBLIC_AGGREGATED_METRIC_REPORTER_H_

#include <array>

#include "base/gtest_prod_util.h"
#include "base/macros.h"
#include "base/metrics/histogram.h"
#include "base/threading/thread_checker.h"
#include "base/time/time.h"
#include "third_party/blink/renderer/platform/platform_export.h"

namespace base {
class HistogramBase;
}

namespace blink {
namespace scheduler {

// A helper class to report metrics split by a specific type.
// This class aggregates smaller value and report when it's over threshold
// to avoid overflows.
//
// |TaskClass| is an enum which should have kCount field.
// All values reported to RecordTask should have lower values.
template <class TaskClass, class ValueType>
class AggregatedMetricReporter {
 public:
  // Aggregation function: takes ValueType, returns the integer value to return
  // to histogram and modifies the passed value.
  // Example: aggregate(time) {
  //   return time.InMilliseconds();
  //   time %= TimeDelta::FromMilliseconds(1);
  // }
  using AggregatorFuncPtr = int (*)(ValueType&);

  AggregatedMetricReporter(const char* metric_name,
                           AggregatorFuncPtr aggregator)
      : AggregatedMetricReporter(
            base::Histogram::FactoryGet(
                metric_name,
                1,
                static_cast<int>(TaskClass::kCount),
                static_cast<int>(TaskClass::kCount) + 1,
                base::HistogramBase::kUmaTargetedHistogramFlag),
            aggregator) {}

  ~AggregatedMetricReporter() {}

  void RecordTask(TaskClass task_class, ValueType value) {
    DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);

    DCHECK_LT(static_cast<int>(task_class),
              static_cast<int>(TaskClass::kCount));

    ValueType& unreported_value =
        unreported_values_[static_cast<int>(task_class)];
    unreported_value += value;

    int value_to_report = aggregator_(unreported_value);
    if (value_to_report > 0) {
      value_per_type_histogram_->AddCount(static_cast<int>(task_class),
                                          value_to_report);
    }
  }

 protected:
  AggregatedMetricReporter(base::HistogramBase* histogram,
                           AggregatorFuncPtr aggregator)
      : value_per_type_histogram_(histogram), aggregator_(aggregator) {}

  std::array<ValueType, static_cast<size_t>(TaskClass::kCount)>
      unreported_values_ = {};
  base::HistogramBase* value_per_type_histogram_;
  AggregatorFuncPtr aggregator_;

  THREAD_CHECKER(thread_checker_);

  DISALLOW_COPY_AND_ASSIGN(AggregatedMetricReporter);
};

}  // namespace scheduler
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_PUBLIC_AGGREGATED_METRIC_REPORTER_H_
