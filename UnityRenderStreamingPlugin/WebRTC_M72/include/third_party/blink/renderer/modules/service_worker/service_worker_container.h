/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_SERVICE_WORKER_SERVICE_WORKER_CONTAINER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_SERVICE_WORKER_SERVICE_WORKER_CONTAINER_H_

#include <memory>
#include "third_party/blink/public/mojom/service_worker/service_worker_registration.mojom-blink.h"
#include "third_party/blink/public/platform/modules/service_worker/web_service_worker_provider.h"
#include "third_party/blink/public/platform/modules/service_worker/web_service_worker_provider_client.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise_property.h"
#include "third_party/blink/renderer/core/dom/context_lifecycle_observer.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/events/event_target.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/modules/service_worker/registration_options.h"
#include "third_party/blink/renderer/modules/service_worker/service_worker.h"
#include "third_party/blink/renderer/modules/service_worker/service_worker_registration.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

namespace blink {

class ExecutionContext;

class MODULES_EXPORT ServiceWorkerContainer final
    : public EventTargetWithInlineData,
      public Supplement<Document>,
      public ContextLifecycleObserver,
      public WebServiceWorkerProviderClient {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(ServiceWorkerContainer);

 public:
  using RegistrationCallbacks =
      WebServiceWorkerProvider::WebServiceWorkerRegistrationCallbacks;

  static const char kSupplementName[];

  static ServiceWorkerContainer* From(Document*);

  static ServiceWorkerContainer* CreateForTesting(
      Document*,
      std::unique_ptr<WebServiceWorkerProvider>);
  ~ServiceWorkerContainer() override;

  void Trace(blink::Visitor*) override;

  ServiceWorker* controller() { return controller_; }
  ScriptPromise ready(ScriptState*);

  ScriptPromise registerServiceWorker(ScriptState*,
                                      const String& pattern,
                                      const RegistrationOptions*);
  ScriptPromise getRegistration(ScriptState*, const String& document_url);
  ScriptPromise getRegistrations(ScriptState*);

  void ContextDestroyed(ExecutionContext*) override;

  // WebServiceWorkerProviderClient implementation.
  void SetController(WebServiceWorkerObjectInfo,
                     bool should_notify_controller_change) override;
  void DispatchMessageEvent(WebServiceWorkerObjectInfo,
                            TransferableMessage) override;
  void CountFeature(mojom::WebFeature) override;

  // EventTarget overrides.
  ExecutionContext* GetExecutionContext() const override;
  const AtomicString& InterfaceName() const override;

  DEFINE_ATTRIBUTE_EVENT_LISTENER(controllerchange, kControllerchange);
  DEFINE_ATTRIBUTE_EVENT_LISTENER(message, kMessage);

  // Returns the ServiceWorkerRegistration object described by the given info.
  // Creates a new object if needed, or else returns the existing one.
  ServiceWorkerRegistration* GetOrCreateServiceWorkerRegistration(
      WebServiceWorkerRegistrationObjectInfo);

  // Returns the ServiceWorker object described by the given info. Creates a new
  // object if needed, or else returns the existing one.
  ServiceWorker* GetOrCreateServiceWorker(WebServiceWorkerObjectInfo);

 private:
  explicit ServiceWorkerContainer(Document*);

  class GetRegistrationForReadyCallback;
  using ReadyProperty =
      ScriptPromiseProperty<Member<ServiceWorkerContainer>,
                            Member<ServiceWorkerRegistration>,
                            Member<ServiceWorkerRegistration>>;
  ReadyProperty* CreateReadyProperty();

  std::unique_ptr<WebServiceWorkerProvider> provider_;
  Member<ServiceWorker> controller_;
  Member<ReadyProperty> ready_;

  // Map from service worker registration id to JavaScript
  // ServiceWorkerRegistration object in current execution context.
  HeapHashMap<int64_t,
              WeakMember<ServiceWorkerRegistration>,
              WTF::IntHash<int64_t>,
              WTF::UnsignedWithZeroKeyHashTraits<int64_t>>
      service_worker_registration_objects_;
  // Map from service worker version id to JavaScript ServiceWorker object in
  // current execution context.
  HeapHashMap<int64_t,
              WeakMember<ServiceWorker>,
              WTF::IntHash<int64_t>,
              WTF::UnsignedWithZeroKeyHashTraits<int64_t>>
      service_worker_objects_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_SERVICE_WORKER_SERVICE_WORKER_CONTAINER_H_
