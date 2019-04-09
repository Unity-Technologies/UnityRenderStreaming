// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CLASS_REQUIRES_FINALIZATION_H_
#define CLASS_REQUIRES_FINALIZATION_H_

#include "heap/stubs.h"

namespace blink {

class A : public GarbageCollected<A> {
public:
    virtual void Trace(Visitor*) { }
};

// Has a non-trivial dtor (user-declared).
class B {
public:
    ~B() { }
    void Trace(Visitor*) { };
};

// Has a trivial dtor.
class C {
public:
    void Trace(Visitor*) { };
};

} // blink namespace

namespace blink {

// Off-heap vectors always need to be finalized.
class NeedsFinalizer : public A {
public:
    void Trace(Visitor*);
private:
    Vector<Member<A> > m_as;
};

// On-heap vectors with inlined objects that need destruction
// need to be finalized.
class AlsoNeedsFinalizer : public A {
public:
    void Trace(Visitor*);
private:
    HeapVector<B, 10> m_bs;
};

// On-heap vectors with no inlined objects never need to be finalized.
class DoesNotNeedFinalizer : public A {
public:
    void Trace(Visitor*);
private:
    HeapVector<B> m_bs;
};

}

#endif
