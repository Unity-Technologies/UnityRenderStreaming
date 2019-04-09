// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_TEST_FUZZER_THREAD_POOL_MANAGER_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_TEST_FUZZER_THREAD_POOL_MANAGER_H_

#include <memory>
#include <vector>

#include "base/synchronization/condition_variable.h"
#include "base/synchronization/lock.h"
#include "base/time/time.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/scheduler/test/fuzzer/proto/sequence_manager_test_description.pb.h"

namespace base {
namespace sequence_manager {

class SequenceManagerFuzzerProcessor;
class SimpleThreadImpl;
class ThreadManager;

// Used by the SequenceManagerFuzzerProcessor to manage threads and synchronize
// their clocks.
class PLATFORM_EXPORT ThreadPoolManager {
 public:
  explicit ThreadPoolManager(SequenceManagerFuzzerProcessor* processor);
  ~ThreadPoolManager();

  // |initial_time| is the time in which the thread is created.
  void CreateThread(
      const google::protobuf::RepeatedPtrField<
          SequenceManagerTestDescription::Action>& initial_thread_actions,
      TimeTicks initial_time);

  // Advances the mock tick clock of all the threads synchronously.
  // Note that this doesn't guarantee advancing the thread's clock to |time|.
  // The clock is advanced to the minimum desired time of all the owned threads.
  void AdvanceClockSynchronouslyToTime(ThreadManager* thread_manager,
                                       TimeTicks time);

  // Advances the mock tick clock of all the threads synchronously.
  // Note that this doesn't guarantee advancing the thread's clock by the next
  // pending task delay. The clock is advanced to the minimum desired time of
  // all the owned threads.
  void AdvanceClockSynchronouslyByPendingTaskDelay(
      ThreadManager* thread_manager);

  // Used by a thread to notify the thread manager that it is done executing the
  // thread actions passed to ThreadPoolManager::CreateThread.
  void ThreadDone();

  void StartInitialThreads();

  // Used by the processor to wait for all of the threads to finish executing
  // the actions passed by ThreadPoolManager::CreateThread. Note that
  // the threads are not terminated until |this| gets destructed.
  void WaitForAllThreads();

  // (Thread Safe) Used to return the thread manager of the |thread_id|'s entry
  // in |threads_| (modulo the number of entries).
  ThreadManager* GetThreadManagerFor(uint64_t thread_id);

  const std::vector<std::unique_ptr<SimpleThreadImpl>>& threads() const;

  SequenceManagerFuzzerProcessor* processor() const;

 private:
  void StartThread(
      const google::protobuf::RepeatedPtrField<
          SequenceManagerTestDescription::Action>& initial_thread_actions,
      ThreadManager* thread_manager);

  // Helper function used by AdvanceClockSynchronouslyToTime and
  // AdvanceClockSynchronouslyByPendingTaskDelay to notify the manager when all
  // threads are ready to compute their next desired time.
  void ThreadReadyToComputeTime();

  // Helper function used by AdvanceClockSynchronouslyToTime and
  // AdvanceClockSynchronouslyByPendingTaskDelay to advance the thread's clock
  // to |next_time_|. Note that this function waits until all threads have
  // voted on the value of |next_time_|.
  void AdvanceThreadClock(ThreadManager* thread_manager);

  // Owner of this class.
  SequenceManagerFuzzerProcessor* const processor_;

  // Used to protect all the members below.
  Lock lock_;

  // Used to synchronize virtual time across all threads.
  TimeTicks next_time_;

  // Condition to ensure that all threads have their desired next time
  // computed, and thus the global |next_time_| can be computed as their
  // minimum value.
  ConditionVariable ready_to_compute_time_;

  // Condition that |next_time_| is computed and that all threads can advance
  // their clock to |next_time_|.
  ConditionVariable ready_to_advance_time_;

  // Condition that all threads are done and the program is ready to
  // terminate.
  ConditionVariable ready_to_terminate_;

  // Condition that threads can start running. This is needed to make sure all
  // of the initial (program entry points) threads were created before any
  // thread starts running.
  ConditionVariable ready_to_execute_threads_;

  // A round starts by all of the threads waiting to compute their desired
  // next time and ends by all of them advancing their clocks.
  ConditionVariable ready_for_next_round_;

  uint64_t threads_waiting_to_compute_time_;
  uint64_t threads_waiting_to_advance_time_;

  // Number of threads done advancing their clocks and are ready for the next
  // round.
  uint64_t threads_ready_for_next_round_;

  // Number of threads done executing their sequence of actions.
  uint64_t threads_ready_to_terminate_;

  // Used to notify the condition |ready_for_next_round_|.
  bool all_threads_ready_;

  // Used to notify the condition |ready_to_start_threads_|.
  bool initial_threads_created_;

  // Threads that are being managed/synchronized. For unit testing purposes,
  // make sure not to create threads at the same time (if the ordering matters)
  // since in this case the order will not be deterministic.
  std::vector<std::unique_ptr<SimpleThreadImpl>> threads_;
};

}  // namespace sequence_manager
}  // namespace base

#endif
