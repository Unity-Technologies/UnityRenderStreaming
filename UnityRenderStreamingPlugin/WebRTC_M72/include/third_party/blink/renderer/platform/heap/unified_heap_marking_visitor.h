// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_HEAP_UNIFIED_HEAP_MARKING_VISITOR_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_HEAP_UNIFIED_HEAP_MARKING_VISITOR_H_

#include "base/macros.h"
#include "third_party/blink/renderer/platform/heap/marking_visitor.h"

namespace blink {

struct WrapperTypeInfo;

// Marking visitor for unified heap garbage collections. Extends the regular
// Oilpan marking visitor by also providing write barriers and visitation
// methods that allow for announcing reachable objects to V8.
class PLATFORM_EXPORT UnifiedHeapMarkingVisitor final : public MarkingVisitor {
 public:
  static std::unique_ptr<UnifiedHeapMarkingVisitor> Create(ThreadState*,
                                                           MarkingMode,
                                                           v8::Isolate*);
  // Write barriers for annotating a write during incremental marking.
  static void WriteBarrier(v8::Isolate*,
                           const TraceWrapperV8Reference<v8::Value>&);
  static void WriteBarrier(v8::Isolate*,
                           DOMWrapperMap<ScriptWrappable>*,
                           ScriptWrappable*);
  static void WriteBarrier(v8::Isolate*, const WrapperTypeInfo*, void*);

  ~UnifiedHeapMarkingVisitor() override = default;

  // Visitation methods that announce reachable wrappers to V8.
  void Visit(const TraceWrapperV8Reference<v8::Value>&) final;
  void Visit(DOMWrapperMap<ScriptWrappable>*, const ScriptWrappable*) final;

 private:
  UnifiedHeapMarkingVisitor(ThreadState*, MarkingMode, v8::Isolate*);

  v8::Isolate* const isolate_;

  DISALLOW_COPY_AND_ASSIGN(UnifiedHeapMarkingVisitor);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_HEAP_UNIFIED_HEAP_MARKING_VISITOR_H_
