// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_FONT_FACE_SET_WORKER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_FONT_FACE_SET_WORKER_H_

#include "base/macros.h"
#include "third_party/blink/renderer/bindings/core/v8/iterable.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise.h"
#include "third_party/blink/renderer/core/css/font_face.h"
#include "third_party/blink/renderer/core/css/font_face_set.h"
#include "third_party/blink/renderer/core/css/offscreen_font_selector.h"
#include "third_party/blink/renderer/core/dom/pausable_object.h"
#include "third_party/blink/renderer/core/workers/worker_global_scope.h"
#include "third_party/blink/renderer/platform/async_method_runner.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace blink {

class Font;

class CORE_EXPORT FontFaceSetWorker final
    : public FontFaceSet,
      public Supplement<WorkerGlobalScope> {
  USING_GARBAGE_COLLECTED_MIXIN(FontFaceSetWorker);

 public:
  static const char kSupplementName[];

  explicit FontFaceSetWorker(WorkerGlobalScope&);
  ~FontFaceSetWorker() override;

  ScriptPromise ready(ScriptState*) override;

  AtomicString status() const override;

  WorkerGlobalScope* GetWorker() const;

  // FontFace::LoadFontCallback
  void NotifyLoaded(FontFace*) override;
  void NotifyError(FontFace*) override;

  void BeginFontLoading(FontFace*);

  static FontFaceSetWorker* From(WorkerGlobalScope&);

  void Trace(Visitor*) override;

 protected:
  bool InActiveContext() const override { return true; }
  FontSelector* GetFontSelector() const override {
    return GetWorker()->GetFontSelector();
  }
  // For workers, this is always an empty list.
  const HeapLinkedHashSet<Member<FontFace>>& CSSConnectedFontFaceList()
      const override {
    DCHECK(
        GetFontSelector()->GetFontFaceCache()->CssConnectedFontFaces().size() ==
        0);
    return GetFontSelector()->GetFontFaceCache()->CssConnectedFontFaces();
  }

  bool ResolveFontStyle(const String&, Font&) override;

 private:
  static FontFaceSetWorker* Create(WorkerGlobalScope& worker) {
    return MakeGarbageCollected<FontFaceSetWorker>(worker);
  }

  void FireDoneEventIfPossible() override;
  DISALLOW_COPY_AND_ASSIGN(FontFaceSetWorker);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_FONT_FACE_SET_WORKER_H_
