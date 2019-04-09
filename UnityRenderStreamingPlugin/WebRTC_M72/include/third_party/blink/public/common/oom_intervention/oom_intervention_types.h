// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_COMMON_OOM_INTERVENTION_OOM_INTERVENTION_TYPES_H_
#define THIRD_PARTY_BLINK_PUBLIC_COMMON_OOM_INTERVENTION_OOM_INTERVENTION_TYPES_H_

#include <stdint.h>

namespace blink {

// The struct with renderer metrics that are used to detect OOMs. This is stored
// in shared memory so that browser can read it even after the renderer dies.
// Use uint64_t for this struct in order to keep the memory layout exactly the
// same across architectures, since it is possible on Android to have browser in
// the arm64 and renderer in the arm32.

struct OomInterventionMetrics {
  uint64_t current_private_footprint_kb;
  uint64_t current_swap_kb;
  uint64_t current_vm_size_kb;

  // Stores the total of V8, BlinkGC and PartitionAlloc memory usage.
  uint64_t current_blink_usage_kb;

  // Indicates whether the crash was because of virtual address space OOM.
  // This holds only 0 or 1 as a value but because of the reason stated above,
  // uses uint64_t instead of boolean.
  uint64_t allocation_failed;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_COMMON_OOM_INTERVENTION_OOM_INTERVENTION_TYPES_H_
