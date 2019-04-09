// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_PRESENTATION_PRESENTATION_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_PRESENTATION_PRESENTATION_H_

#include "third_party/blink/renderer/core/dom/context_lifecycle_observer.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/heap/heap.h"

namespace blink {

class LocalFrame;
class PresentationReceiver;
class PresentationRequest;

// Implements the main entry point of the Presentation API corresponding to the
// Presentation.idl
// See https://w3c.github.io/presentation-api/#navigatorpresentation for
// details.
class Presentation final : public ScriptWrappable, public ContextClient {
  USING_GARBAGE_COLLECTED_MIXIN(Presentation);
  DEFINE_WRAPPERTYPEINFO();

 public:
  static Presentation* Create(LocalFrame*);

  void Trace(blink::Visitor*) override;

  PresentationRequest* defaultRequest() const;
  void setDefaultRequest(PresentationRequest*);

  PresentationReceiver* receiver();

 private:
  explicit Presentation(LocalFrame*);

  // Default PresentationRequest used by the embedder.
  Member<PresentationRequest> default_request_;

  // PresentationReceiver instance. It will always be nullptr if the Blink
  // instance is not running as a presentation receiver.
  Member<PresentationReceiver> receiver_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_PRESENTATION_PRESENTATION_H_
