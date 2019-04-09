// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CLASS_MULTIPLE_TRACE_BASES_H_
#define CLASS_MULTIPLE_TRACE_BASES_H_

#include "heap/stubs.h"

namespace blink {

class Base : public GarbageCollected<Base> {
public:
    virtual void Trace(Visitor*);
};

class Mixin1 : public GarbageCollectedMixin {
public:
    void Trace(Visitor*);
};

class Mixin2 : public GarbageCollectedMixin {
public:
    void Trace(Visitor*);
};

class Derived1 : public Base, public Mixin1 {
    USING_GARBAGE_COLLECTED_MIXIN(Derived1);
    // Requires Trace method.
};

class Derived2 : public Base, public Mixin1, public Mixin2 {
    USING_GARBAGE_COLLECTED_MIXIN(Derived2);
    void Trace(Visitor*) override;
};

}

#endif
