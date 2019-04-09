// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_INSPECTOR_DEVTOOLS_SESSION_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_INSPECTOR_DEVTOOLS_SESSION_H_

#include "base/macros.h"
#include "mojo/public/cpp/bindings/associated_binding.h"
#include "third_party/blink/public/web/devtools_agent.mojom-blink.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/inspector/inspector_session_state.h"
#include "third_party/blink/renderer/core/inspector/protocol/Forward.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"
#include "v8/include/v8-inspector-protocol.h"

namespace blink {

class DevToolsAgent;
class DocumentLoader;
class InspectorAgent;
class LocalFrame;

class CORE_EXPORT DevToolsSession
    : public GarbageCollectedFinalized<DevToolsSession>,
      public mojom::blink::DevToolsSession,
      public protocol::FrontendChannel,
      public v8_inspector::V8Inspector::Channel {
 public:
  DevToolsSession(
      DevToolsAgent*,
      mojom::blink::DevToolsSessionHostAssociatedPtrInfo host_ptr_info,
      mojom::blink::DevToolsSessionAssociatedRequest main_request,
      mojom::blink::DevToolsSessionRequest io_request,
      mojom::blink::DevToolsSessionStatePtr reattach_session_state);
  ~DevToolsSession() override;

  void ConnectToV8(v8_inspector::V8Inspector*, int context_group_id);
  v8_inspector::V8InspectorSession* V8Session() { return v8_session_.get(); }

  void Append(InspectorAgent*);
  void Detach();
  void FlushProtocolNotifications();
  void Trace(blink::Visitor*);

  // Core probes.
  void DidStartProvisionalLoad(LocalFrame*);
  void DidFailProvisionalLoad(LocalFrame*);
  void DidCommitLoad(LocalFrame*, DocumentLoader*);

 private:
  class IOSession;

  // mojom::blink::DevToolsSession implementation.
  void DispatchProtocolCommand(int call_id,
                               const String& method,
                               const String& message) override;

  // protocol::FrontendChannel implementation.
  void sendProtocolResponse(
      int call_id,
      std::unique_ptr<protocol::Serializable> message) override;
  void sendProtocolNotification(
      std::unique_ptr<protocol::Serializable> message) override;
  void fallThrough(int call_id,
                   const String& method,
                   const String& message) override;
  void flushProtocolNotifications() override;

  // v8_inspector::V8Inspector::Channel implementation.
  void sendResponse(
      int call_id,
      std::unique_ptr<v8_inspector::StringBuffer> message) override;
  void sendNotification(
      std::unique_ptr<v8_inspector::StringBuffer> message) override;

  bool IsDetached();
  void SendProtocolResponse(int call_id, const String& message);

  Member<DevToolsAgent> agent_;
  mojo::AssociatedBinding<mojom::blink::DevToolsSession> binding_;
  mojom::blink::DevToolsSessionHostAssociatedPtr host_ptr_;
  IOSession* io_session_;
  std::unique_ptr<v8_inspector::V8InspectorSession> v8_session_;
  std::unique_ptr<protocol::UberDispatcher> inspector_backend_dispatcher_;
  InspectorSessionState session_state_;
  HeapVector<Member<InspectorAgent>> agents_;
  class Notification;
  Vector<std::unique_ptr<Notification>> notification_queue_;
  InspectorAgentState v8_session_state_;
  InspectorAgentState::String v8_session_state_json_;

  DISALLOW_COPY_AND_ASSIGN(DevToolsSession);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_INSPECTOR_DEVTOOLS_SESSION_H_
