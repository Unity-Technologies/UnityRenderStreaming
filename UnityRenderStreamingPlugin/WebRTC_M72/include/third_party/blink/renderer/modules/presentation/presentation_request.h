// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_PRESENTATION_PRESENTATION_REQUEST_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_PRESENTATION_PRESENTATION_REQUEST_H_

#include "third_party/blink/renderer/bindings/core/v8/active_script_wrappable.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise.h"
#include "third_party/blink/renderer/core/dom/context_lifecycle_observer.h"
#include "third_party/blink/renderer/core/dom/events/event_target.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/modules/presentation/presentation_promise_property.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/heap/heap.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

// Implements the PresentationRequest interface from the Presentation API from
// which websites can start or join presentation connections.
class MODULES_EXPORT PresentationRequest final
    : public EventTargetWithInlineData,
      public ActiveScriptWrappable<PresentationRequest>,
      public ContextClient {
  USING_GARBAGE_COLLECTED_MIXIN(PresentationRequest);
  DEFINE_WRAPPERTYPEINFO();

 public:
  ~PresentationRequest() override = default;

  static PresentationRequest* Create(ExecutionContext*,
                                     const String& url,
                                     ExceptionState&);
  static PresentationRequest* Create(ExecutionContext*,
                                     const Vector<String>& urls,
                                     ExceptionState&);

  // EventTarget implementation.
  const AtomicString& InterfaceName() const override;
  ExecutionContext* GetExecutionContext() const override;

  // ScriptWrappable implementation.
  bool HasPendingActivity() const final;

  ScriptPromise start(ScriptState*);
  ScriptPromise reconnect(ScriptState*, const String& id);
  ScriptPromise getAvailability(ScriptState*);

  const Vector<KURL>& Urls() const;

  DEFINE_ATTRIBUTE_EVENT_LISTENER(connectionavailable, kConnectionavailable);

  void Trace(blink::Visitor*) override;

 protected:
  // EventTarget implementation.
  void AddedEventListener(const AtomicString& event_type,
                          RegisteredEventListener&) override;

 private:
  PresentationRequest(ExecutionContext*, const Vector<KURL>&);

  Member<PresentationAvailabilityProperty> availability_property_;
  Vector<KURL> urls_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_PRESENTATION_PRESENTATION_REQUEST_H_
