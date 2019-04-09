// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TEST_SCOPED_MOCK_CLOCK_OVERRIDE_H_
#define BASE_TEST_SCOPED_MOCK_CLOCK_OVERRIDE_H_

#include <memory>

#include "base/macros.h"
#include "base/time/time.h"
#include "base/time/time_override.h"

namespace base {

// Override the return value of Time::Now(), Time::NowFromSystemTime(),
// TimeTicks::Now(), and ThreadTicks::Now() through a simple advanceable clock.
//
// This utility is intended to support tests that:
//
//   - Depend on large existing codebases that call TimeXYZ::Now() directly or
//   - Have no ability to inject a TickClock into the code getting the time
//     (e.g. integration tests in which a TickClock would be several layers
//     removed from the test code)
//
// For new unit tests, developers are highly encouraged to structure new code
// around a dependency injected base::Clock, base::TickClock, etc. to be able
// to supply a mock time in tests without a global override.
//
// NOTE: ScopedMockClockOverride should be created while single-threaded and
// before the first call to Now() to avoid threading issues and inconsistencies
// in returned values. Nested overrides are not allowed.
class ScopedMockClockOverride {
 public:
  ScopedMockClockOverride();
  ~ScopedMockClockOverride();

  static Time Now();
  static TimeTicks NowTicks();
  static ThreadTicks NowThreadTicks();

  void Advance(TimeDelta delta);

 private:
  std::unique_ptr<base::subtle::ScopedTimeClockOverrides> time_clock_overrides_;
  TimeDelta offset_;
  static ScopedMockClockOverride* scoped_mock_clock_;

  DISALLOW_COPY_AND_ASSIGN(ScopedMockClockOverride);
};

}  // namespace base

#endif  // BASE_TEST_SCOPED_MOCK_CLOCK_OVERRIDE_H_
