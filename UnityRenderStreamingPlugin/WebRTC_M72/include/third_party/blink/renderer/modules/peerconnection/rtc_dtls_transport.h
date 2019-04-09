// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_PEERCONNECTION_RTC_DTLS_TRANSPORT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_PEERCONNECTION_RTC_DTLS_TRANSPORT_H_

#include "third_party/blink/renderer/bindings/core/v8/active_script_wrappable.h"
#include "third_party/blink/renderer/core/dom/context_lifecycle_observer.h"
#include "third_party/blink/renderer/modules/event_target_modules.h"

namespace blink {

class DOMArrayBuffer;
class RTCIceTransport;

enum class RTCDtlsTransportState {
  kNew,
  kChecking,
  kConnected,
  kCompleted,
  kDisconnected,
  kFailed,
  kClosed
};

// Blink bindings for the RTCDtlsTransport JavaScript object.
//
class MODULES_EXPORT RTCDtlsTransport final : public EventTargetWithInlineData,
                                              public ContextClient {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(RTCDtlsTransport);

 public:
  static RTCDtlsTransport* Create(ExecutionContext* context);

  ~RTCDtlsTransport() override;

  // rtc_dtls_transport.idl
  RTCIceTransport* iceTransport() const;
  String state() const;
  const HeapVector<Member<DOMArrayBuffer>>& getRemoteCertificates() const;

  DEFINE_ATTRIBUTE_EVENT_LISTENER(statechange, kStatechange);
  DEFINE_ATTRIBUTE_EVENT_LISTENER(error, kError);

  // EventTarget overrides.
  const AtomicString& InterfaceName() const override;
  ExecutionContext* GetExecutionContext() const override;
  // For garbage collection.
  void Trace(blink::Visitor* visitor) override;

 private:
  explicit RTCDtlsTransport(ExecutionContext* context);
  HeapVector<Member<DOMArrayBuffer>> remote_certificates_;
};

}  // namespace blink
#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_PEERCONNECTION_RTC_DTLS_TRANSPORT_H_
