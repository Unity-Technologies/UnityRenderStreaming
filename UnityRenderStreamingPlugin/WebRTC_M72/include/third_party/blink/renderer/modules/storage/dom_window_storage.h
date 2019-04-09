// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_STORAGE_DOM_WINDOW_STORAGE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_STORAGE_DOM_WINDOW_STORAGE_H_

#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/supplementable.h"

namespace blink {

class ExceptionState;
class LocalDOMWindow;
class StorageArea;

class DOMWindowStorage final : public GarbageCollected<DOMWindowStorage>,
                               public Supplement<LocalDOMWindow> {
  USING_GARBAGE_COLLECTED_MIXIN(DOMWindowStorage);

 public:
  static const char kSupplementName[];

  static DOMWindowStorage& From(LocalDOMWindow&);
  static StorageArea* sessionStorage(LocalDOMWindow&, ExceptionState&);
  static StorageArea* localStorage(LocalDOMWindow&, ExceptionState&);

  explicit DOMWindowStorage(LocalDOMWindow&);

  StorageArea* sessionStorage(ExceptionState&) const;
  StorageArea* localStorage(ExceptionState&) const;
  StorageArea* OptionalSessionStorage() const { return session_storage_.Get(); }
  StorageArea* OptionalLocalStorage() const { return local_storage_.Get(); }

  void Trace(blink::Visitor*) override;

 private:
  mutable Member<StorageArea> session_storage_;
  mutable Member<StorageArea> local_storage_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_STORAGE_DOM_WINDOW_STORAGE_H_
