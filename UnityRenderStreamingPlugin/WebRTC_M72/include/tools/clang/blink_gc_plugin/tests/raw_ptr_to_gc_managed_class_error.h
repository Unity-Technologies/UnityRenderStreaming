// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RAW_PTR_TO_GC_MANAGED_CLASS_ERROR_H_
#define RAW_PTR_TO_GC_MANAGED_CLASS_ERROR_H_

#include "heap/stubs.h"

namespace blink {

class HeapObject;

class PartObject {
    DISALLOW_NEW();
private:
    PartObject();

    HeapObject* m_rawObj;
    HeapObject& m_refObj;
};

class HeapObject : public GarbageCollected<HeapObject> {
public:
    void Trace(Visitor*);
private:
    PartObject m_part;
    HeapVector<HeapObject*> m_objs;
};

}

#endif
