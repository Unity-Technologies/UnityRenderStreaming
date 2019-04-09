// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_ELEMENT_TIMING_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_ELEMENT_TIMING_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/dom_high_res_time_stamp.h"
#include "third_party/blink/renderer/core/geometry/dom_rect_read_only.h"
#include "third_party/blink/renderer/core/timing/performance_entry.h"

namespace blink {

// The PerformanceElementTiming object exposes the time in which an element is
// first rendered on the screen and its intersection with the viewport at the
// time it is painted. Currently this is only done for <img> elements but other
// element types should be supported in the future.
class CORE_EXPORT PerformanceElementTiming final : public PerformanceEntry {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static PerformanceElementTiming* Create(const AtomicString& name,
                                          const IntRect& intersection_rect,
                                          DOMHighResTimeStamp start_time);

  ~PerformanceElementTiming() override;

  AtomicString entryType() const override;
  PerformanceEntryType EntryTypeEnum() const override;

  DOMRectReadOnly* intersectionRect() const { return intersection_rect_; }

  void Trace(blink::Visitor*) override;

 private:
  PerformanceElementTiming(const AtomicString& name,
                           const IntRect& intersection_rect,
                           DOMHighResTimeStamp start_time);

  void BuildJSONValue(V8ObjectBuilder&) const override;

  Member<DOMRectReadOnly> intersection_rect_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_ELEMENT_TIMING_H_
