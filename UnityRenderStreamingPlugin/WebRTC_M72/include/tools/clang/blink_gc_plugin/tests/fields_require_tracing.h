// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FIELDS_REQUIRE_TRACING_H_
#define FIELDS_REQUIRE_TRACING_H_

#include "heap/stubs.h"

namespace blink {

class HeapObject;
class PartObject;

class PartBObject {
    DISALLOW_NEW();
public:
    void Trace(Visitor*);
private:
    HeapHashSet<PartBObject> m_set;
    HeapVector<PartBObject> m_vector;
};

class PartObject {
    DISALLOW_NEW();
public:
    void Trace(Visitor*);
private:
    Member<HeapObject> m_obj1;
    Member<HeapObject> m_obj2;
    Member<HeapObject> m_obj3;

    HeapVector<PartBObject> m_parts;
};

class HeapObject : public GarbageCollected<HeapObject> {
public:
    void Trace(Visitor*);
private:
    PartObject m_part;
    Member<HeapObject> m_obj;
};

}

#endif
