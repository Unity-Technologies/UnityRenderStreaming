// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_ANIMATIONWORKLET_CSS_ANIMATION_WORKLET_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_ANIMATIONWORKLET_CSS_ANIMATION_WORKLET_H_

#include "third_party/blink/renderer/core/dom/context_lifecycle_observer.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/modules/animationworklet/animation_worklet.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/supplementable.h"

namespace blink {

class Document;

class MODULES_EXPORT CSSAnimationWorklet final
    : public GarbageCollected<CSSAnimationWorklet>,
      public Supplement<LocalDOMWindow>,
      public ContextLifecycleObserver {
  USING_GARBAGE_COLLECTED_MIXIN(CSSAnimationWorklet);

 public:
  static const char kSupplementName[];

  static AnimationWorklet* animationWorklet(ScriptState*);

  explicit CSSAnimationWorklet(Document*);

  void ContextDestroyed(ExecutionContext*) override;

  void Trace(blink::Visitor*) override;

 private:
  static CSSAnimationWorklet& From(LocalDOMWindow&);

  Member<AnimationWorklet> animation_worklet_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_ANIMATIONWORKLET_CSS_ANIMATION_WORKLET_H_
