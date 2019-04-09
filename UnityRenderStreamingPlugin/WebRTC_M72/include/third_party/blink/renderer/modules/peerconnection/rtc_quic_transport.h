// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_PEERCONNECTION_RTC_QUIC_TRANSPORT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_PEERCONNECTION_RTC_QUIC_TRANSPORT_H_

#include "third_party/blink/renderer/core/dom/context_lifecycle_observer.h"
#include "third_party/blink/renderer/modules/event_target_modules.h"
#include "third_party/blink/renderer/modules/peerconnection/adapters/quic_transport_proxy.h"
#include "third_party/blink/renderer/modules/peerconnection/rtc_ice_transport.h"
#include "third_party/blink/renderer/modules/peerconnection/rtc_quic_parameters.h"

namespace blink {

class DOMArrayBuffer;
class ExceptionState;
class RTCCertificate;
class RTCQuicStream;
class P2PQuicTransportFactory;

enum class RTCQuicTransportState {
  kNew,
  kConnecting,
  kConnected,
  kClosed,
  kFailed
};

// The RTCQuicTransport does not need to be ActiveScriptWrappable since the
// RTCIceTransport to which it is attached holds a strong reference to it as
// long as it is alive.
class MODULES_EXPORT RTCQuicTransport final
    : public EventTargetWithInlineData,
      public ContextClient,
      public QuicTransportProxy::Delegate {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(RTCQuicTransport);

 public:
  enum class CloseReason {
    // stop() was called.
    kLocalStopped,
    // The remote side closed the QUIC connection.
    kRemoteStopped,
    // The QUIC connection failed.
    kFailed,
    // The RTCIceTransport was closed.
    kIceTransportClosed,
    // The ExecutionContext is being destroyed.
    kContextDestroyed,
  };

  static RTCQuicTransport* Create(
      ExecutionContext* context,
      RTCIceTransport* transport,
      const HeapVector<Member<RTCCertificate>>& certificates,
      ExceptionState& exception_state);
  static RTCQuicTransport* Create(
      ExecutionContext* context,
      RTCIceTransport* transport,
      const HeapVector<Member<RTCCertificate>>& certificates,
      ExceptionState& exception_state,
      std::unique_ptr<P2PQuicTransportFactory> p2p_quic_transport_factory);

  ~RTCQuicTransport() override;

  // Called by the RTCIceTransport when it is being closed.
  void OnIceTransportClosed(RTCIceTransport::CloseReason reason);

  // Called by the RTCIceTransport when its start() method is called.
  void OnIceTransportStarted();

  RTCQuicStream* AddStream(QuicStreamProxy* stream_proxy);
  void RemoveStream(RTCQuicStream* stream);

  // https://w3c.github.io/webrtc-quic/#quic-transport*
  RTCIceTransport* transport() const;
  String state() const;
  RTCQuicParameters* getLocalParameters() const;
  RTCQuicParameters* getRemoteParameters() const;
  const HeapVector<Member<RTCCertificate>>& getCertificates() const;
  const HeapVector<Member<DOMArrayBuffer>>& getRemoteCertificates() const;
  void start(const RTCQuicParameters* remote_parameters,
             ExceptionState& exception_state);
  void stop();
  RTCQuicStream* createStream(ExceptionState& exception_state);
  DEFINE_ATTRIBUTE_EVENT_LISTENER(statechange, kStatechange);
  DEFINE_ATTRIBUTE_EVENT_LISTENER(error, kError);
  DEFINE_ATTRIBUTE_EVENT_LISTENER(quicstream, kQuicstream);

  // EventTarget overrides.
  const AtomicString& InterfaceName() const override;
  ExecutionContext* GetExecutionContext() const override;

  // For garbage collection.
  void Trace(blink::Visitor* visitor) override;

 private:
  RTCQuicTransport(
      ExecutionContext* context,
      RTCIceTransport* transport,
      const HeapVector<Member<RTCCertificate>>& certificates,
      ExceptionState& exception_state,
      std::unique_ptr<P2PQuicTransportFactory> p2p_quic_transport_factory);

  // QuicTransportProxy::Delegate overrides;
  void OnConnected() override;
  void OnConnectionFailed(const std::string& error_details,
                          bool from_remote) override;
  void OnRemoteStopped() override;
  void OnStream(QuicStreamProxy* stream_proxy) override;

  // Starts the underlying QUIC connection.
  void StartConnection();

  // Permenantly closes the RTCQuicTransport with the given reason.
  // The RTCQuicTransport must not already be closed or failed.
  // This will transition the state to either closed or failed according to the
  // reason.
  void Close(CloseReason reason);

  bool IsClosed() const { return state_ == RTCQuicTransportState::kClosed; }
  bool RaiseExceptionIfClosed(ExceptionState& exception_state) const;

  Member<RTCIceTransport> transport_;
  RTCQuicTransportState state_ = RTCQuicTransportState::kNew;
  HeapVector<Member<RTCCertificate>> certificates_;
  HeapVector<Member<DOMArrayBuffer>> remote_certificates_;
  Member<RTCQuicParameters> remote_parameters_;
  std::unique_ptr<P2PQuicTransportFactory> p2p_quic_transport_factory_;
  std::unique_ptr<QuicTransportProxy> proxy_;
  HeapHashSet<Member<RTCQuicStream>> streams_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_PEERCONNECTION_RTC_QUIC_TRANSPORT_H_
