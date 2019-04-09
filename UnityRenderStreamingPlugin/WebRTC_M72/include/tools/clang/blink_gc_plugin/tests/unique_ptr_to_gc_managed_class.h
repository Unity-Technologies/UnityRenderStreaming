// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OWN_PTR_TO_GC_MANAGED_CLASS_H_
#define OWN_PTR_TO_GC_MANAGED_CLASS_H_

#include "heap/stubs.h"

namespace blink {

class HeapObject;

class PartObject {
    DISALLOW_NEW();
private:
    std::unique_ptr<HeapObject> m_obj;
};

class HeapObject : public GarbageCollectedFinalized<HeapObject> {
public:
    void Trace(Visitor*);
private:
    Vector<std::unique_ptr<HeapObject> > m_objs;
    std::unique_ptr<HeapVector<Member<HeapObject> > > m_objs2;
};

}

#endif
