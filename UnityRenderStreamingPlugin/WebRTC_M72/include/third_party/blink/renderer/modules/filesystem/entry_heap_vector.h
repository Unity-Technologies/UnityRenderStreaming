// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_FILESYSTEM_ENTRY_HEAP_VECTOR_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_FILESYSTEM_ENTRY_HEAP_VECTOR_H_

#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class Entry;

using EntryHeapVector = HeapVector<Member<Entry>>;

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_FILESYSTEM_ENTRY_HEAP_VECTOR_H_
