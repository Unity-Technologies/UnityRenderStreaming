/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EXPORTED_WEB_DEV_TOOLS_AGENT_IMPL_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EXPORTED_WEB_DEV_TOOLS_AGENT_IMPL_H_

#include <memory>

#include "third_party/blink/public/platform/web_size.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/inspector/devtools_agent.h"
#include "third_party/blink/renderer/core/inspector/inspector_layer_tree_agent.h"
#include "third_party/blink/renderer/core/inspector/inspector_page_agent.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/scheduler/public/thread.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

namespace blink {

class ClientMessageLoopAdapter;
class GraphicsLayer;
class InspectedFrames;
class InspectorNetworkAgent;
class InspectorOverlayAgent;
class InspectorResourceContainer;
class InspectorResourceContentLoader;
class LocalFrame;
class WebLocalFrameImpl;

class CORE_EXPORT WebDevToolsAgentImpl final
    : public GarbageCollectedFinalized<WebDevToolsAgentImpl>,
      public DevToolsAgent::Client,
      public InspectorPageAgent::Client,
      public InspectorLayerTreeAgent::Client,
      private Thread::TaskObserver {
 public:
  class WorkerClient {
   public:
    virtual ~WorkerClient() {}
    virtual void ResumeStartup() = 0;
  };

  static WebDevToolsAgentImpl* CreateForFrame(WebLocalFrameImpl*);
  static WebDevToolsAgentImpl* CreateForWorker(WebLocalFrameImpl*,
                                               WorkerClient*);
  ~WebDevToolsAgentImpl() override;
  virtual void Trace(blink::Visitor*);
  DevToolsAgent* GetDevToolsAgent() const { return agent_.Get(); }

  void WillBeDestroyed();
  void FlushProtocolNotifications();
  void UpdateOverlays();
  bool HandleInputEvent(const WebInputEvent&);
  void DispatchBufferedTouchEvents();
  void BindRequest(mojom::blink::DevToolsAgentHostAssociatedPtrInfo,
                   mojom::blink::DevToolsAgentAssociatedRequest);

  // Instrumentation from web/ layer.
  void DidCommitLoadForLocalFrame(LocalFrame*);
  bool ScreencastEnabled();
  String NavigationInitiatorInfo(LocalFrame*);
  String EvaluateInOverlayForTesting(const String& script);

 private:
  friend class ClientMessageLoopAdapter;

  WebDevToolsAgentImpl(WebLocalFrameImpl*,
                       bool include_view_agents,
                       WorkerClient*);

  // DevToolsAgent::Client implementation.
  void AttachSession(DevToolsSession*, bool restore) override;
  void DetachSession(DevToolsSession*) override;
  void InspectElement(const WebPoint& point_in_local_root) override;
  void DebuggerTaskStarted() override;
  void DebuggerTaskFinished() override;

  // InspectorPageAgent::Client implementation.
  void PageLayoutInvalidated(bool resized) override;

  // InspectorLayerTreeAgent::Client implementation.
  bool IsInspectorLayer(GraphicsLayer*) override;

  // Thread::TaskObserver implementation.
  void WillProcessTask(const base::PendingTask&) override;
  void DidProcessTask(const base::PendingTask&) override;

  Member<DevToolsAgent> agent_;
  HeapHashMap<Member<DevToolsSession>, Member<InspectorNetworkAgent>>
      network_agents_;
  HeapHashMap<Member<DevToolsSession>, Member<InspectorPageAgent>> page_agents_;
  HeapHashMap<Member<DevToolsSession>, Member<InspectorOverlayAgent>>
      overlay_agents_;
  WorkerClient* worker_client_;
  Member<WebLocalFrameImpl> web_local_frame_impl_;
  Member<CoreProbeSink> probe_sink_;
  Member<InspectorResourceContentLoader> resource_content_loader_;
  Member<InspectedFrames> inspected_frames_;
  Member<InspectorResourceContainer> resource_container_;
  Member<Node> node_to_inspect_;
  bool include_view_agents_;
};

}  // namespace blink

#endif
