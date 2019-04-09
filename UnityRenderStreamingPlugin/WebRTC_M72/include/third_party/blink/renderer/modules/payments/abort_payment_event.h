// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_PAYMENTS_ABORT_PAYMENT_EVENT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_PAYMENTS_ABORT_PAYMENT_EVENT_H_

#include "third_party/blink/renderer/modules/event_modules.h"
#include "third_party/blink/renderer/modules/service_worker/extendable_event.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/noncopyable.h"

namespace WTF {
class AtomicString;
}

namespace blink {

class ExtendableEventInit;
class RespondWithObserver;
class ScriptState;

class MODULES_EXPORT AbortPaymentEvent final : public ExtendableEvent {
  DEFINE_WRAPPERTYPEINFO();
  WTF_MAKE_NONCOPYABLE(AbortPaymentEvent);

 public:
  static AbortPaymentEvent* Create(const AtomicString& type,
                                   const ExtendableEventInit*);
  static AbortPaymentEvent* Create(const AtomicString& type,
                                   const ExtendableEventInit*,
                                   RespondWithObserver*,
                                   WaitUntilObserver*);
  ~AbortPaymentEvent() override;

  const AtomicString& InterfaceName() const override;

  void respondWith(ScriptState*, ScriptPromise, ExceptionState&);

  void Trace(blink::Visitor*) override;

 private:
  AbortPaymentEvent(const AtomicString& type,
                    const ExtendableEventInit*,
                    RespondWithObserver*,
                    WaitUntilObserver*);

  Member<RespondWithObserver> observer_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_PAYMENTS_ABORT_PAYMENT_EVENT_H_
