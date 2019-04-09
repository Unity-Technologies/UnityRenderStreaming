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
#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_DEDICATED_WORKER_THREAD_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_DEDICATED_WORKER_THREAD_H_

#include <memory>
#include "third_party/blink/renderer/core/workers/worker_thread.h"

namespace blink {

class DedicatedWorkerObjectProxy;
struct GlobalScopeCreationParams;

class CORE_EXPORT DedicatedWorkerThread : public WorkerThread {
 public:
  static std::unique_ptr<DedicatedWorkerThread> Create(
      const String& name,
      ExecutionContext* parent_execution_context,
      DedicatedWorkerObjectProxy&);
  ~DedicatedWorkerThread() override;

  WorkerBackingThread& GetWorkerBackingThread() override {
    return *worker_backing_thread_;
  }
  void ClearWorkerBackingThread() override;
  DedicatedWorkerObjectProxy& WorkerObjectProxy() const {
    return worker_object_proxy_;
  }

  bool IsNestedWorker() const { return is_nested_worker_; }

 private:
  friend class DedicatedWorkerThreadForTest;

  DedicatedWorkerThread(const String& name,
                        ExecutionContext* parent_execution_context,
                        DedicatedWorkerObjectProxy&);
  WorkerOrWorkletGlobalScope* CreateWorkerGlobalScope(
      std::unique_ptr<GlobalScopeCreationParams>) override;

  WebThreadType GetThreadType() const override {
    return WebThreadType::kDedicatedWorkerThread;
  }

  std::unique_ptr<WorkerBackingThread> worker_backing_thread_;
  const String name_;
  DedicatedWorkerObjectProxy& worker_object_proxy_;
  const bool is_nested_worker_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_DEDICATED_WORKER_THREAD_H_
