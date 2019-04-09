// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EVENTS_PROMISE_REJECTION_EVENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EVENTS_PROMISE_REJECTION_EVENT_H_

#include "third_party/blink/renderer/bindings/core/v8/script_promise.h"
#include "third_party/blink/renderer/bindings/core/v8/script_value.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/events/event.h"
#include "third_party/blink/renderer/core/events/promise_rejection_event_init.h"
#include "third_party/blink/renderer/platform/bindings/dom_wrapper_world.h"
#include "third_party/blink/renderer/platform/bindings/script_state.h"
#include "third_party/blink/renderer/platform/bindings/trace_wrapper_v8_reference.h"

namespace blink {

class CORE_EXPORT PromiseRejectionEvent final : public Event {
  DEFINE_WRAPPERTYPEINFO();
  USING_PRE_FINALIZER(PromiseRejectionEvent, Dispose);

 public:
  static PromiseRejectionEvent* Create(
      ScriptState* state,
      const AtomicString& type,
      const PromiseRejectionEventInit* initializer) {
    return new PromiseRejectionEvent(state, type, initializer);
  }

  ScriptValue reason(ScriptState*) const;
  ScriptPromise promise(ScriptState*) const;

  const AtomicString& InterfaceName() const override;

  // PromiseRejectionEvents are similar to ErrorEvents in that they can't be
  // observed across different worlds.
  bool CanBeDispatchedInWorld(const DOMWrapperWorld&) const override;

  void Trace(blink::Visitor*) override;

 private:
  PromiseRejectionEvent(ScriptState*,
                        const AtomicString&,
                        const PromiseRejectionEventInit*);
  ~PromiseRejectionEvent() override;
  void Dispose();

  scoped_refptr<DOMWrapperWorld> world_;
  TraceWrapperV8Reference<v8::Value> promise_;
  TraceWrapperV8Reference<v8::Value> reason_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_EVENTS_PROMISE_REJECTION_EVENT_H_
