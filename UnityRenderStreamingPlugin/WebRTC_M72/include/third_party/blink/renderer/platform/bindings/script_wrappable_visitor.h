// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_SCRIPT_WRAPPABLE_VISITOR_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_SCRIPT_WRAPPABLE_VISITOR_H_

#include "third_party/blink/renderer/platform/heap/visitor.h"
#include "third_party/blink/renderer/platform/platform_export.h"

namespace blink {

// Abstract visitor for visiting ScriptWrappable. Inherit from this
// visitor and implement the remaining Visit*() methods to visit all
// references related to wrappers.
class PLATFORM_EXPORT ScriptWrappableVisitor : public Visitor {
 public:
  explicit ScriptWrappableVisitor(ThreadState* thread_state)
      : Visitor(thread_state) {}

  // Unused blink::Visitor overrides. Derived visitors should still override
  // the cross-component visitation methods. See Visitor documentation.
  void Visit(void* object, TraceDescriptor desc) final {}
  void VisitWeak(void* object,
                 void** object_slot,
                 TraceDescriptor desc,
                 WeakCallback callback) final {}
  void VisitBackingStoreWeakly(void*,
                               void**,
                               TraceDescriptor,
                               WeakCallback,
                               void*) final {}
  void VisitBackingStoreOnly(void*, void**) final {}
  void RegisterBackingStoreCallback(void**, MovingObjectCallback, void*) final {
  }
  void RegisterWeakCallback(void*, WeakCallback) final {}

 protected:
  using Visitor::Visit;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_SCRIPT_WRAPPABLE_VISITOR_H_
