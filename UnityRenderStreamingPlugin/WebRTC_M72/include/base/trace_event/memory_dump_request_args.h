// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TRACE_EVENT_MEMORY_DUMP_REQUEST_ARGS_H_
#define BASE_TRACE_EVENT_MEMORY_DUMP_REQUEST_ARGS_H_

// This file defines the types and structs used to issue memory dump requests.
// These are also used in the IPCs for coordinating inter-process memory dumps.

#include <stdint.h>
#include <map>
#include <memory>
#include <string>

#include "base/base_export.h"
#include "base/callback.h"
#include "base/optional.h"
#include "base/process/process_handle.h"

namespace base {
namespace trace_event {

class ProcessMemoryDump;

// Captures the reason why a memory dump is being requested. This is to allow
// selective enabling of dumps, filtering and post-processing. Keep this
// consistent with memory_instrumentation.mojo and
// memory_instrumentation_struct_traits.{h,cc}
enum class MemoryDumpType {
  PERIODIC_INTERVAL,     // Dumping memory at periodic intervals.
  EXPLICITLY_TRIGGERED,  // Non maskable dump request.
  SUMMARY_ONLY,          // Calculate just the summary & don't add to the trace.
  LAST = SUMMARY_ONLY
};

// Tells the MemoryDumpProvider(s) how much detailed their dumps should be.
// Keep this consistent with memory_instrumentation.mojo and
// memory_instrumentation_struct_traits.{h,cc}
enum class MemoryDumpLevelOfDetail : uint32_t {
  FIRST,

  // For background tracing mode. The dump time is quick, and typically just the
  // totals are expected. Suballocations need not be specified. Dump name must
  // contain only pre-defined strings and string arguments cannot be added.
  BACKGROUND = FIRST,

  // For the levels below, MemoryDumpProvider instances must guarantee that the
  // total size reported in the root node is consistent. Only the granularity of
  // the child MemoryAllocatorDump(s) differs with the levels.

  // Few entries, typically a fixed number, per dump.
  LIGHT,

  // Unrestricted amount of entries per dump.
  DETAILED,

  LAST = DETAILED
};

// Keep this consistent with memory_instrumentation.mojo and
// memory_instrumentation_struct_traits.{h,cc}
struct BASE_EXPORT MemoryDumpRequestArgs {
  // Globally unique identifier. In multi-process dumps, all processes issue a
  // local dump with the same guid. This allows the trace importers to
  // reconstruct the global dump.
  uint64_t dump_guid;

  MemoryDumpType dump_type;
  MemoryDumpLevelOfDetail level_of_detail;
};

// Args for ProcessMemoryDump and passed to OnMemoryDump calls for memory dump
// providers. Dump providers are expected to read the args for creating dumps.
struct MemoryDumpArgs {
  // Specifies how detailed the dumps should be.
  MemoryDumpLevelOfDetail level_of_detail;

  // Globally unique identifier. In multi-process dumps, all processes issue a
  // local dump with the same guid. This allows the trace importers to
  // reconstruct the global dump.
  uint64_t dump_guid;
};

using ProcessMemoryDumpCallback = Callback<
    void(bool success, uint64_t dump_guid, std::unique_ptr<ProcessMemoryDump>)>;

BASE_EXPORT const char* MemoryDumpTypeToString(const MemoryDumpType& dump_type);

BASE_EXPORT MemoryDumpType StringToMemoryDumpType(const std::string& str);

BASE_EXPORT const char* MemoryDumpLevelOfDetailToString(
    const MemoryDumpLevelOfDetail& level_of_detail);

BASE_EXPORT MemoryDumpLevelOfDetail
StringToMemoryDumpLevelOfDetail(const std::string& str);

}  // namespace trace_event
}  // namespace base

#endif  // BASE_TRACE_EVENT_MEMORY_DUMP_REQUEST_ARGS_H_
