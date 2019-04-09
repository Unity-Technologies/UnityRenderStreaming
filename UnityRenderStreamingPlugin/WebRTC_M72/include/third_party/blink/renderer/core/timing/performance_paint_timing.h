// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_PAINT_TIMING_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_PAINT_TIMING_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/timing/performance_entry.h"

namespace blink {

class CORE_EXPORT PerformancePaintTiming final : public PerformanceEntry {
  DEFINE_WRAPPERTYPEINFO();

 public:
  enum class PaintType { kFirstPaint, kFirstContentfulPaint };

  PerformancePaintTiming(PaintType, double start_time);
  ~PerformancePaintTiming() override;

  AtomicString entryType() const override;
  PerformanceEntryType EntryTypeEnum() const override;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_PAINT_TIMING_H_
