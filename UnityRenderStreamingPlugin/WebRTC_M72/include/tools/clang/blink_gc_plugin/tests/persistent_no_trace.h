// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PERSISTENT_NO_TRACE_H_
#define PERSISTENT_NO_TRACE_H_

#include "heap/stubs.h"

namespace blink {

class HeapObject : public GarbageCollected<HeapObject> {
public:
    void Trace(Visitor*);
private:
    CrossThreadPersistent<HeapObject> m_crossThreadPersistent;
    CrossThreadWeakPersistent<HeapObject> m_crossThreadWeakPersistent;
};

}

#endif
