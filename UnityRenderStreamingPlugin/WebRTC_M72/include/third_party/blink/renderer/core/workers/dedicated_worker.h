// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_DEDICATED_WORKER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_DEDICATED_WORKER_H_

#include "base/memory/scoped_refptr.h"
#include "third_party/blink/renderer/bindings/core/v8/active_script_wrappable.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/events/event_listener.h"
#include "third_party/blink/renderer/core/dom/events/event_target.h"
#include "third_party/blink/renderer/core/dom/pausable_object.h"
#include "third_party/blink/renderer/core/messaging/message_port.h"
#include "third_party/blink/renderer/core/workers/abstract_worker.h"
#include "third_party/blink/renderer/core/workers/worker_options.h"
#include "third_party/blink/renderer/platform/graphics/begin_frame_provider.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

namespace v8_inspector {
struct V8StackTraceId;
}  // namespace v8_inspector

namespace blink {

class DedicatedWorkerMessagingProxy;
class ExceptionState;
class ExecutionContext;
class PostMessageOptions;
class ScriptState;
class WorkerClassicScriptLoader;
class WorkerClients;
struct GlobalScopeCreationParams;

// Implementation of the Worker interface defined in the WebWorker HTML spec:
// https://html.spec.whatwg.org/multipage/workers.html#worker
//
// Confusingly, the Worker interface is for dedicated workers, so this class is
// called DedicatedWorker. This lives on the main thread.
class CORE_EXPORT DedicatedWorker final
    : public AbstractWorker,
      public ActiveScriptWrappable<DedicatedWorker> {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(DedicatedWorker);
  // Eager finalization is needed to notify the parent object destruction of the
  // GC-managed messaging proxy and to initiate worker termination.
  EAGERLY_FINALIZE();

 public:
  static DedicatedWorker* Create(ExecutionContext*,
                                 const String& url,
                                 const WorkerOptions*,
                                 ExceptionState&);

  DedicatedWorker(ExecutionContext*,
                  const KURL& script_request_url,
                  const WorkerOptions*);
  ~DedicatedWorker() override;

  void postMessage(ScriptState*,
                   const ScriptValue& message,
                   Vector<ScriptValue>& transfer,
                   ExceptionState&);
  void postMessage(ScriptState*,
                   const ScriptValue& message,
                   const PostMessageOptions*,
                   ExceptionState&);
  void terminate();
  BeginFrameProviderParams CreateBeginFrameProviderParams();

  // Implements ContextLifecycleObserver (via AbstractWorker).
  void ContextDestroyed(ExecutionContext*) override;

  // Implements ScriptWrappable
  // (via AbstractWorker -> EventTargetWithInlineData -> EventTarget).
  bool HasPendingActivity() const final;

  // Returns the name specified by WorkerOptions.
  const String Name() const;

  DEFINE_ATTRIBUTE_EVENT_LISTENER(message, kMessage);

  void Trace(blink::Visitor*) override;

 private:
  // Starts the worker.
  void Start();

  std::unique_ptr<GlobalScopeCreationParams> CreateGlobalScopeCreationParams(
      const KURL& script_url);

  WorkerClients* CreateWorkerClients();

  // Callbacks for |classic_script_loader_|.
  void OnResponse();
  void OnFinished(const v8_inspector::V8StackTraceId&);

  // Implements EventTarget (via AbstractWorker -> EventTargetWithInlineData).
  const AtomicString& InterfaceName() const final;

  const KURL script_request_url_;
  Member<const WorkerOptions> options_;
  const Member<DedicatedWorkerMessagingProxy> context_proxy_;

  Member<WorkerClassicScriptLoader> classic_script_loader_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_DEDICATED_WORKER_H_
