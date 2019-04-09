// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_HEAP_NAME_TRAITS_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_HEAP_NAME_TRAITS_H_

#include "third_party/blink/renderer/platform/bindings/name_client.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace blink {

template <typename T>
class NameTrait {
  STATIC_ONLY(NameTrait);

 public:
  static const char* GetName(const void* obj) {
    return GetNameFor(static_cast<const T*>(obj));
  }

 private:
  static const char* GetNameFor(const NameClient* wrapper_tracable) {
    return wrapper_tracable->NameInHeapSnapshot();
  }

  static const char* GetNameFor(...) { return "InternalNode"; }
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_HEAP_NAME_TRAITS_H_
