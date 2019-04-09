// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_PUSH_MESSAGING_PUSH_MANAGER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_PUSH_MESSAGING_PUSH_MANAGER_H_

#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class ExceptionState;
class PushSubscriptionOptionsInit;
class ScriptPromise;
class ScriptState;
class ServiceWorkerRegistration;

class MODULES_EXPORT PushManager final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static PushManager* Create(ServiceWorkerRegistration* registration) {
    return new PushManager(registration);
  }

  // Web-exposed property:
  static Vector<String> supportedContentEncodings();

  // Web-exposed methods:
  ScriptPromise subscribe(ScriptState* script_state,
                          const PushSubscriptionOptionsInit* options,
                          ExceptionState& exception_state);
  ScriptPromise getSubscription(ScriptState* script_state);
  ScriptPromise permissionState(ScriptState* script_state,
                                const PushSubscriptionOptionsInit* options,
                                ExceptionState& exception_state);

  void Trace(blink::Visitor* visitor) override;

 private:
  explicit PushManager(ServiceWorkerRegistration* registration);

  Member<ServiceWorkerRegistration> registration_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_PUSH_MESSAGING_PUSH_MANAGER_H_
