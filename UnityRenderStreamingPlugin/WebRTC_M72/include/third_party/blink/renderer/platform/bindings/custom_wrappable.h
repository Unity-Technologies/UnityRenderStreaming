// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_CUSTOM_WRAPPABLE_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_CUSTOM_WRAPPABLE_H_

#include "third_party/blink/renderer/platform/bindings/name_client.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/noncopyable.h"

namespace blink {

// See the comment on CustomWrappableAdaptor.
class PLATFORM_EXPORT CustomWrappable
    : public GarbageCollectedFinalized<CustomWrappable>,
      public NameClient {
  WTF_MAKE_NONCOPYABLE(CustomWrappable);

 public:
  virtual ~CustomWrappable() = default;
  virtual void Trace(Visitor*) {}
  const char* NameInHeapSnapshot() const override { return "CustomWrappable"; }

 protected:
  CustomWrappable() = default;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_CUSTOM_WRAPPABLE_H_
