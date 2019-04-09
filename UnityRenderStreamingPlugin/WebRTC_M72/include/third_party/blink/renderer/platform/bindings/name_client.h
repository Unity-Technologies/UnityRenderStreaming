// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_NAME_CLIENT_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_NAME_CLIENT_H_

#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/noncopyable.h"

namespace blink {

// Provides classes with a human-readable name that can be used for inspecting
// the object graph.
class PLATFORM_EXPORT NameClient {
  WTF_MAKE_NONCOPYABLE(NameClient);

 public:
  NameClient() = default;
  ~NameClient() = default;

  // Human-readable name of this object. The DevTools heap snapshot uses
  // this method to show the object.
  virtual const char* NameInHeapSnapshot() const = 0;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_NAME_CLIENT_H_
