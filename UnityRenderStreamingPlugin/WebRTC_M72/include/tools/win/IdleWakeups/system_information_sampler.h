// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TOOLS_WIN_IDLEWAKEUPS_SYSTEM_INFORMATION_SAMPLER_H_
#define TOOLS_WIN_IDLEWAKEUPS_SYSTEM_INFORMATION_SAMPLER_H_

#include <map>
#include <memory>
#include <vector>

#include <windows.h>

// SYSTEM_PROCESS_INFORMATION and SYSTEM_THREAD_INFORMATION structures
// use HANDLE for the thread / process IDs.
typedef HANDLE ThreadId;
typedef HANDLE ProcessId;

// Contains per thread data stored in each data snapshot.
struct ThreadData {
  ThreadId thread_id;
  ULONG context_switches;
};

typedef std::vector<ThreadData> ThreadsVector;

// Contains per process data stored in each data snapshot.
struct ProcessData {
  ULONGLONG cpu_time;
  ULONGLONG working_set;
  ThreadsVector threads;
};

typedef std::map<ProcessId, ProcessData> ProcessDataMap;

struct ProcessDataSnapshot {
  ProcessDataMap processes;
  double timestamp;
};

class SystemInformationSampler {
 public:
  SystemInformationSampler(const wchar_t* process_name);
  ~SystemInformationSampler();

  std::unique_ptr<ProcessDataSnapshot> TakeSnapshot();

  const wchar_t* target_process_name_filter() const {
    return target_process_name_;
  }

 private:
  wchar_t target_process_name_[256] = {};
  LARGE_INTEGER perf_frequency_;
  LARGE_INTEGER initial_counter_;
  size_t previous_buffer_size_ = 0;

  SystemInformationSampler& operator=(const SystemInformationSampler&) = delete;
  SystemInformationSampler(const SystemInformationSampler&) = delete;
};

#endif  // TOOLS_WIN_IDLEWAKEUPS_SYSTEM_INFORMATION_SAMPLER_H_