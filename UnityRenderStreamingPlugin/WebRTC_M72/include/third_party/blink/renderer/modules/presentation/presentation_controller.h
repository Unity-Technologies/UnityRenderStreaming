// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_PRESENTATION_PRESENTATION_CONTROLLER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_PRESENTATION_PRESENTATION_CONTROLLER_H_

#include "mojo/public/cpp/bindings/binding.h"
#include "services/service_manager/public/cpp/interface_provider.h"
#include "third_party/blink/public/mojom/presentation/presentation.mojom-blink.h"
#include "third_party/blink/renderer/core/dom/context_lifecycle_observer.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_frame_client.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/modules/presentation/presentation.h"
#include "third_party/blink/renderer/modules/presentation/presentation_availability_callbacks.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/supplementable.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class ControllerPresentationConnection;
class PresentationAvailabilityObserver;
class PresentationAvailabilityState;

// Implements the PresentationController interface from the Presentation API
// from which websites can implement the controlling side of a presentation.
class MODULES_EXPORT PresentationController
    : public GarbageCollectedFinalized<PresentationController>,
      public Supplement<LocalFrame>,
      public ContextLifecycleObserver,
      public mojom::blink::PresentationController {
  USING_GARBAGE_COLLECTED_MIXIN(PresentationController);
  WTF_MAKE_NONCOPYABLE(PresentationController);

 public:
  static const char kSupplementName[];

  ~PresentationController() override;

  static PresentationController* From(LocalFrame&);

  static void ProvideTo(LocalFrame&);

  static PresentationController* FromContext(ExecutionContext*);

  // Implementation of Supplement.
  void Trace(blink::Visitor*) override;

  // Called by the Presentation object to advertize itself to the controller.
  // The Presentation object is kept as a WeakMember in order to avoid keeping
  // it alive when it is no longer in the tree.
  void SetPresentation(Presentation*);

  // Handling of running connections.
  void RegisterConnection(ControllerPresentationConnection*);

  // Return a connection in |m_connections| with id equals to |presentationId|,
  // url equals to one of |presentationUrls|, and state is not terminated.
  // Return null if such a connection does not exist.
  ControllerPresentationConnection* FindExistingConnection(
      const blink::WebVector<blink::WebURL>& presentation_urls,
      const blink::WebString& presentation_id);

  // Returns a reference to the PresentationService ptr, requesting the remote
  // service if needed. May return an invalid ptr if the associated Document is
  // detached.
  mojom::blink::PresentationServicePtr& GetPresentationService();

  // Returns the PresentationAvailabilityState owned by |this|, creating it if
  // needed. Always non-null.
  PresentationAvailabilityState* GetAvailabilityState();

  // Marked virtual for testing.
  virtual void AddAvailabilityObserver(PresentationAvailabilityObserver*);
  virtual void RemoveAvailabilityObserver(PresentationAvailabilityObserver*);

 protected:
  PresentationController(LocalFrame&);

 private:
  // Implementation of ContextLifecycleObserver.
  void ContextDestroyed(ExecutionContext*) override;

  // mojom::blink::PresentationController implementation.
  void OnScreenAvailabilityUpdated(const KURL&,
                                   mojom::blink::ScreenAvailability) override;
  void OnConnectionStateChanged(
      mojom::blink::PresentationInfoPtr,
      mojom::blink::PresentationConnectionState) override;
  void OnConnectionClosed(mojom::blink::PresentationInfoPtr,
                          mojom::blink::PresentationConnectionCloseReason,
                          const String& message) override;
  void OnDefaultPresentationStarted(
      mojom::blink::PresentationConnectionResultPtr result) override;

  // Return the connection associated with the given |presentation_info| or
  // null if it doesn't exist.
  ControllerPresentationConnection* FindConnection(
      const mojom::blink::PresentationInfo&) const;

  // Lazily-instantiated when the page queries for availability.
  std::unique_ptr<PresentationAvailabilityState> availability_state_;

  // The Presentation instance associated with that frame.
  WeakMember<Presentation> presentation_;

  // The presentation connections associated with that frame.
  HeapHashSet<WeakMember<ControllerPresentationConnection>> connections_;

  // Lazily-initialized pointer to PresentationService.
  mojom::blink::PresentationServicePtr presentation_service_;

  // Lazily-initialized binding for mojom::blink::PresentationController. Sent
  // to |presentation_service_|'s implementation.
  mojo::Binding<mojom::blink::PresentationController> controller_binding_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_PRESENTATION_PRESENTATION_CONTROLLER_H_
