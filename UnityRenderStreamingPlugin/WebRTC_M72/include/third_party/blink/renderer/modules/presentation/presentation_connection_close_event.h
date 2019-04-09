// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_PRESENTATION_PRESENTATION_CONNECTION_CLOSE_EVENT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_PRESENTATION_PRESENTATION_CONNECTION_CLOSE_EVENT_H_

#include "third_party/blink/renderer/modules/event_modules.h"
#include "third_party/blink/renderer/modules/presentation/presentation_connection.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class PresentationConnectionCloseEventInit;

// Presentation API event to be fired when the state of a PresentationConnection
// has changed to 'closed'.
class PresentationConnectionCloseEvent final : public Event {
  DEFINE_WRAPPERTYPEINFO();

 public:
  ~PresentationConnectionCloseEvent() override = default;

  static PresentationConnectionCloseEvent* Create(
      const AtomicString& event_type,
      const String& reason,
      const String& message) {
    return new PresentationConnectionCloseEvent(event_type, reason, message);
  }

  static PresentationConnectionCloseEvent* Create(
      const AtomicString& event_type,
      const PresentationConnectionCloseEventInit* initializer) {
    return new PresentationConnectionCloseEvent(event_type, initializer);
  }

  const String& reason() const { return reason_; }
  const String& message() const { return message_; }

  const AtomicString& InterfaceName() const override;

  void Trace(blink::Visitor*) override;

 private:
  PresentationConnectionCloseEvent(const AtomicString& event_type,
                                   const String& reason,
                                   const String& message);
  PresentationConnectionCloseEvent(
      const AtomicString& event_type,
      const PresentationConnectionCloseEventInit* initializer);

  String reason_;
  String message_;
};

}  // namespace blink

#endif  // PresentationConnectionAvailableEvent_h
