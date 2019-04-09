// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_PUSH_MESSAGING_PUSH_EVENT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_PUSH_MESSAGING_PUSH_EVENT_H_

#include "third_party/blink/renderer/modules/event_modules.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/modules/push_messaging/push_message_data.h"
#include "third_party/blink/renderer/modules/service_worker/extendable_event.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/text/atomic_string.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class PushEventInit;

class MODULES_EXPORT PushEvent final : public ExtendableEvent {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static PushEvent* Create(const AtomicString& type,
                           PushMessageData* data,
                           WaitUntilObserver* observer) {
    return new PushEvent(type, data, observer);
  }
  static PushEvent* Create(const AtomicString& type,
                           const PushEventInit* initializer) {
    return new PushEvent(type, initializer);
  }

  ~PushEvent() override;

  // ExtendableEvent interface.
  const AtomicString& InterfaceName() const override;

  PushMessageData* data();

  void Trace(blink::Visitor* visitor) override;

 private:
  PushEvent(const AtomicString& type,
            PushMessageData* data,
            WaitUntilObserver* observer);
  PushEvent(const AtomicString& type, const PushEventInit* initializer);

  Member<PushMessageData> data_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_PUSH_MESSAGING_PUSH_EVENT_H_
