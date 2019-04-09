// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LEFT_MOST_GC_BASE_H_
#define LEFT_MOST_GC_BASE_H_

#include "heap/stubs.h"

namespace blink {

class A { };
class B { };

class Right : public A, public B, public GarbageCollected<Right> { };  // Error
class Left : public GarbageCollected<Left>, public B, public A { };

class DerivedRight : public Right, public Left { };  // Error
class DerivedLeft : public Left, public Right { };

class C : public GarbageCollected<C> {
public:
    virtual void Trace(Visitor*);
};

class IllFormed : public A, public C { }; // Error

class LeftMixin : public GarbageCollectedMixin {
public:
    virtual void Trace(Visitor*);
};

class DerivedLeftMixin : public LeftMixin, public GarbageCollected<DerivedLeftMixin> {
  USING_GARBAGE_COLLECTED_MIXIN(DerivedLeftMixin);
};

}

#endif
