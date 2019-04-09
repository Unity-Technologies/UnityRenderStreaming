/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_DEDICATED_WORKER_OBJECT_PROXY_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_DEDICATED_WORKER_OBJECT_PROXY_H_

#include <memory>
#include "base/macros.h"
#include "base/memory/scoped_refptr.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/messaging/blink_transferable_message.h"
#include "third_party/blink/renderer/core/messaging/message_port.h"
#include "third_party/blink/renderer/core/workers/threaded_object_proxy_base.h"
#include "third_party/blink/renderer/core/workers/worker_reporting_proxy.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class DedicatedWorkerMessagingProxy;
class ParentExecutionContextTaskRunners;
class ThreadedMessagingProxyBase;
class WorkerThread;

// A proxy class to talk to a DedicatedWorker object on the main thread via the
// DedicatedWorkerMessagingProxy from a worker thread. See class comments on
// ThreadedObjectProxyBase.h for the lifetime and thread affinity.
class CORE_EXPORT DedicatedWorkerObjectProxy : public ThreadedObjectProxyBase {
  USING_FAST_MALLOC(DedicatedWorkerObjectProxy);

 public:
  static std::unique_ptr<DedicatedWorkerObjectProxy> Create(
      DedicatedWorkerMessagingProxy*,
      ParentExecutionContextTaskRunners*);
  ~DedicatedWorkerObjectProxy() override;

  void PostMessageToWorkerObject(BlinkTransferableMessage);
  void ProcessUnhandledException(int exception_id, WorkerThread*);
  void ProcessMessageFromWorkerObject(BlinkTransferableMessage, WorkerThread*);

  // ThreadedObjectProxyBase overrides.
  void ReportException(const String& error_message,
                       std::unique_ptr<SourceLocation>,
                       int exception_id) override;
  void DidEvaluateClassicScript(bool success) override;
  void DidEvaluateModuleScript(bool success) override;

 protected:
  DedicatedWorkerObjectProxy(DedicatedWorkerMessagingProxy*,
                             ParentExecutionContextTaskRunners*);

  CrossThreadWeakPersistent<ThreadedMessagingProxyBase> MessagingProxyWeakPtr()
      final;

 private:
  friend class DedicatedWorkerObjectProxyForTest;

  // No guarantees about the lifetimes of tasks posted by this proxy wrt the
  // DedicatedWorkerMessagingProxy so a weak pointer must be used when posting
  // the tasks.
  CrossThreadWeakPersistent<DedicatedWorkerMessagingProxy>
      messaging_proxy_weak_ptr_;

  DISALLOW_COPY_AND_ASSIGN(DedicatedWorkerObjectProxy);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_DEDICATED_WORKER_OBJECT_PROXY_H_
