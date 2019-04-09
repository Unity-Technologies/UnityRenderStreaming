// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TRACE_AFTER_DISPATCH_IMPL_ERROR_H_
#define TRACE_AFTER_DISPATCH_IMPL_ERROR_H_

#include "heap/stubs.h"

namespace blink {

class X : public GarbageCollected<X> {
 public:
  void Trace(Visitor*) {}
};

enum ClassTag {
  BASE, DERIVED
};

class TraceAfterDispatchInlinedBase
    : public GarbageCollected<TraceAfterDispatchInlinedBase> {
 public:
  explicit TraceAfterDispatchInlinedBase(ClassTag tag) : tag_(tag) {}

  void Trace(Visitor*);

  void TraceAfterDispatch(Visitor* visitor) {
    // No Trace call; should get a warning.
  }

 private:
  ClassTag tag_;
  Member<X> x_base_;
};

class TraceAfterDispatchInlinedDerived : public TraceAfterDispatchInlinedBase {
 public:
  TraceAfterDispatchInlinedDerived() : TraceAfterDispatchInlinedBase(DERIVED) {}

  void TraceAfterDispatch(Visitor* visitor) {
    // No Trace call (for member and base class).
  }

 private:
  Member<X> x_derived_;
};

class TraceAfterDispatchExternBase
    : public GarbageCollected<TraceAfterDispatchExternBase> {
 public:
  explicit TraceAfterDispatchExternBase(ClassTag tag) : tag_(tag) {}

  void Trace(Visitor* visitor);

  void TraceAfterDispatch(Visitor* visitor);

 private:
  ClassTag tag_;
  Member<X> x_base_;
};

class TraceAfterDispatchExternDerived : public TraceAfterDispatchExternBase {
 public:
  TraceAfterDispatchExternDerived() : TraceAfterDispatchExternBase(DERIVED) {}

  void TraceAfterDispatch(Visitor* visitor);

 private:
  Member<X> x_derived_;
};

}

#endif  // TRACE_AFTER_DISPATCH_IMPL_ERROR_H_
