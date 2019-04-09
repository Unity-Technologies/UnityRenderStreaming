// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_THREADED_MESSAGING_PROXY_BASE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_THREADED_MESSAGING_PROXY_BASE_H_

#include "base/optional.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/frame/web_feature_forward.h"
#include "third_party/blink/renderer/core/inspector/console_types.h"
#include "third_party/blink/renderer/core/workers/parent_execution_context_task_runners.h"
#include "third_party/blink/renderer/core/workers/worker_backing_thread_startup_data.h"
#include "third_party/blink/renderer/core/workers/worker_thread.h"
#include "third_party/blink/renderer/platform/heap/self_keep_alive.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

namespace base {

class WaitableEvent;

}  // namespace base

namespace blink {

class ExecutionContext;
class SourceLocation;
struct GlobalScopeCreationParams;

// The base proxy class to talk to Worker/WorkletGlobalScope on a worker thread
// from the parent context thread (Note that this is always the main thread for
// now because nested worker is not supported yet). This must be created,
// accessed and destroyed on the parent context thread.
//
// This has a unique lifetime: this is co-owned by the parent object (e.g.,
// DedicatedWorker, AnimationWorklet) and by itself via SelfKeepAlive. The
// parent object releases the reference on its destructor and SelfKeepAlive is
// cleared when the worker thread is terminated.
//
// This co-ownership is necessary because the proxy needs to outlive components
// living on the worker thread (e.g., WorkerGlobalScope) but the parent object
// can be destroyed before the completion of worker thread termination.
class CORE_EXPORT ThreadedMessagingProxyBase
    : public GarbageCollectedFinalized<ThreadedMessagingProxyBase> {
 public:
  virtual ~ThreadedMessagingProxyBase();

  void TerminateGlobalScope();

  // This method should be called in the destructor of the object which
  // initially created it. This object could either be a Worker or a Worklet.
  // This may cause deletion of this via |keep_alive_|.
  void ParentObjectDestroyed();

  void CountFeature(WebFeature);
  void CountDeprecation(WebFeature);

  void ReportConsoleMessage(MessageSource,
                            MessageLevel,
                            const String& message,
                            std::unique_ptr<SourceLocation>);

  void WorkerThreadTerminated();

  // Number of live messaging proxies, used by leak detection.
  static int ProxyCount();

  virtual void Trace(blink::Visitor*);

 protected:
  explicit ThreadedMessagingProxyBase(ExecutionContext*);

  void InitializeWorkerThread(
      std::unique_ptr<GlobalScopeCreationParams>,
      const base::Optional<WorkerBackingThreadStartupData>&);

  ExecutionContext* GetExecutionContext() const;
  ParentExecutionContextTaskRunners* GetParentExecutionContextTaskRunners()
      const;

  // May return nullptr after termination is requested.
  WorkerThread* GetWorkerThread() const;

  bool AskedToTerminate() const { return asked_to_terminate_; }

  // Returns true if this is called on the parent context thread.
  bool IsParentContextThread() const;

 private:
  virtual std::unique_ptr<WorkerThread> CreateWorkerThread() = 0;

  Member<ExecutionContext> execution_context_;

  // Accessed cross-thread when worker thread posts tasks to the parent.
  CrossThreadPersistent<ParentExecutionContextTaskRunners>
      parent_execution_context_task_runners_;

  std::unique_ptr<WorkerThread> worker_thread_;

  bool asked_to_terminate_ = false;

  // Used to terminate the synchronous resource loading (XMLHttpRequest) on the
  // worker thread from the main thread.
  base::WaitableEvent terminate_sync_load_event_;

  // Used to keep this alive until the worker thread gets terminated. This is
  // necessary because the co-owner (i.e., Worker or Worklet object) can be
  // destroyed before thread termination.
  SelfKeepAlive<ThreadedMessagingProxyBase> keep_alive_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_THREADED_MESSAGING_PROXY_BASE_H_
