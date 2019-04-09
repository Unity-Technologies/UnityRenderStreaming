// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_BACKGROUND_FETCH_BACKGROUND_FETCH_EVENT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_BACKGROUND_FETCH_BACKGROUND_FETCH_EVENT_H_

#include "third_party/blink/public/platform/modules/background_fetch/background_fetch.mojom-blink.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/modules/service_worker/extendable_event.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/text/atomic_string.h"

namespace blink {

class BackgroundFetchEventInit;
class BackgroundFetchRegistration;
class WaitUntilObserver;

class MODULES_EXPORT BackgroundFetchEvent : public ExtendableEvent {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static BackgroundFetchEvent* Create(
      const AtomicString& type,
      const BackgroundFetchEventInit* initializer) {
    return new BackgroundFetchEvent(type, initializer, nullptr /* observer */);
  }

  static BackgroundFetchEvent* Create(
      const AtomicString& type,
      const BackgroundFetchEventInit* initializer,
      WaitUntilObserver* observer) {
    return new BackgroundFetchEvent(type, initializer, observer);
  }

  ~BackgroundFetchEvent() override;

  // Web Exposed attribute defined in the IDL file.
  BackgroundFetchRegistration* registration() const;

  // ExtendableEvent interface.
  const AtomicString& InterfaceName() const override;

  void Trace(blink::Visitor* visitor) override;

 protected:
  BackgroundFetchEvent(const AtomicString& type,
                       const BackgroundFetchEventInit* initializer,
                       WaitUntilObserver* observer);

  // Corresponds to the 'registration' attribute in the idl.
  Member<BackgroundFetchRegistration> registration_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_BACKGROUND_FETCH_BACKGROUND_FETCH_EVENT_H_
