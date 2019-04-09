// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_INSPECTOR_DEVTOOLS_AGENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_INSPECTOR_DEVTOOLS_AGENT_H_

#include <memory>

#include "base/single_thread_task_runner.h"
#include "base/unguessable_token.h"
#include "mojo/public/cpp/bindings/associated_binding.h"
#include "mojo/public/cpp/bindings/binding.h"
#include "third_party/blink/public/web/devtools_agent.mojom-blink.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/ref_counted.h"

namespace blink {

class CoreProbeSink;
class DevToolsSession;
class ExecutionContext;
class InspectedFrames;
class InspectorTaskRunner;
class WorkerThread;
struct WorkerDevToolsParams;

class CORE_EXPORT DevToolsAgent
    : public GarbageCollectedFinalized<DevToolsAgent>,
      public mojom::blink::DevToolsAgent {
 public:
  class Client {
   public:
    virtual ~Client() {}
    virtual void AttachSession(DevToolsSession*, bool restore) = 0;
    virtual void DetachSession(DevToolsSession*) = 0;
    virtual void InspectElement(const WebPoint&) = 0;
    virtual void DebuggerTaskStarted() = 0;
    virtual void DebuggerTaskFinished() = 0;
  };

  static std::unique_ptr<WorkerDevToolsParams> WorkerThreadCreated(
      ExecutionContext* parent_context,
      WorkerThread*,
      const KURL&);
  static void WorkerThreadTerminated(ExecutionContext* parent_context,
                                     WorkerThread*);

  DevToolsAgent(Client*,
                InspectedFrames*,
                CoreProbeSink*,
                scoped_refptr<InspectorTaskRunner> inspector_task_runner,
                scoped_refptr<base::SingleThreadTaskRunner> io_task_runner);
  ~DevToolsAgent() override;

  void Dispose();
  void FlushProtocolNotifications();
  void BindRequest(mojom::blink::DevToolsAgentHostPtrInfo,
                   mojom::blink::DevToolsAgentRequest,
                   scoped_refptr<base::SingleThreadTaskRunner>);
  void BindRequest(mojom::blink::DevToolsAgentHostAssociatedPtrInfo,
                   mojom::blink::DevToolsAgentAssociatedRequest,
                   scoped_refptr<base::SingleThreadTaskRunner>);
  virtual void Trace(blink::Visitor*);

 private:
  friend class DevToolsSession;

  // mojom::blink::DevToolsAgent implementation.
  void AttachDevToolsSession(
      mojom::blink::DevToolsSessionHostAssociatedPtrInfo,
      mojom::blink::DevToolsSessionAssociatedRequest main_session,
      mojom::blink::DevToolsSessionRequest io_session,
      mojom::blink::DevToolsSessionStatePtr reattach_session_state) override;
  void InspectElement(const WebPoint& point) override;
  void ReportChildWorkers(bool report, bool wait_for_debugger) override;

  struct WorkerData {
    KURL url;
    mojom::blink::DevToolsAgentPtr agent_ptr;
    mojom::blink::DevToolsAgentHostRequest host_request;
    base::UnguessableToken devtools_worker_token;
    bool waiting_for_debugger;
  };
  void ReportChildWorker(std::unique_ptr<WorkerData>);

  void CleanupConnection();

  Client* client_;
  mojo::Binding<mojom::blink::DevToolsAgent> binding_;
  mojo::AssociatedBinding<mojom::blink::DevToolsAgent> associated_binding_;
  mojom::blink::DevToolsAgentHostPtr host_ptr_;
  mojom::blink::DevToolsAgentHostAssociatedPtr associated_host_ptr_;
  Member<InspectedFrames> inspected_frames_;
  Member<CoreProbeSink> probe_sink_;
  HeapHashSet<Member<DevToolsSession>> sessions_;
  scoped_refptr<InspectorTaskRunner> inspector_task_runner_;
  scoped_refptr<base::SingleThreadTaskRunner> io_task_runner_;
  HashMap<WorkerThread*, std::unique_ptr<WorkerData>>
      unreported_child_worker_threads_;
  bool report_child_workers_ = false;
  bool pause_child_workers_on_start_ = false;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_INSPECTOR_DEVTOOLS_AGENT_H_
