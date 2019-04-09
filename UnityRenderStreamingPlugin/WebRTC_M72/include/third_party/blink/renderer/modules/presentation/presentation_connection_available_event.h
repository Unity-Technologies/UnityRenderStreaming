// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_PRESENTATION_PRESENTATION_CONNECTION_AVAILABLE_EVENT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_PRESENTATION_PRESENTATION_CONNECTION_AVAILABLE_EVENT_H_

#include "third_party/blink/renderer/modules/event_modules.h"
#include "third_party/blink/renderer/modules/presentation/presentation_connection.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class PresentationConnectionAvailableEventInit;

// Presentation API event to be fired when a presentation has been triggered
// by the embedder using the default presentation URL and id.
// See https://code.google.com/p/chromium/issues/detail?id=459001 for details.
class PresentationConnectionAvailableEvent final : public Event {
  DEFINE_WRAPPERTYPEINFO();

 public:
  ~PresentationConnectionAvailableEvent() override;

  static PresentationConnectionAvailableEvent* Create(
      const AtomicString& event_type,
      PresentationConnection* connection) {
    return new PresentationConnectionAvailableEvent(event_type, connection);
  }
  static PresentationConnectionAvailableEvent* Create(
      const AtomicString& event_type,
      const PresentationConnectionAvailableEventInit* initializer) {
    return new PresentationConnectionAvailableEvent(event_type, initializer);
  }

  PresentationConnection* connection() { return connection_.Get(); }

  const AtomicString& InterfaceName() const override;

  void Trace(blink::Visitor*) override;

 private:
  PresentationConnectionAvailableEvent(const AtomicString& event_type,
                                       PresentationConnection*);
  PresentationConnectionAvailableEvent(
      const AtomicString& event_type,
      const PresentationConnectionAvailableEventInit* initializer);

  Member<PresentationConnection> connection_;
};

DEFINE_TYPE_CASTS(
    PresentationConnectionAvailableEvent,
    Event,
    event,
    event->InterfaceName() ==
        event_interface_names::kPresentationConnectionAvailableEvent,
    event.InterfaceName() ==
        event_interface_names::kPresentationConnectionAvailableEvent);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_PRESENTATION_PRESENTATION_CONNECTION_AVAILABLE_EVENT_H_
