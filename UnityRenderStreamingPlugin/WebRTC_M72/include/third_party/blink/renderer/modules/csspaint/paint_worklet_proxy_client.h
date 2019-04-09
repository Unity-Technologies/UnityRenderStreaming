// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_CSSPAINT_PAINT_WORKLET_PROXY_CLIENT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_CSSPAINT_PAINT_WORKLET_PROXY_CLIENT_H_

#include "base/macros.h"
#include "base/single_thread_task_runner.h"
#include "third_party/blink/renderer/core/workers/worker_clients.h"
#include "third_party/blink/renderer/modules/csspaint/paint_worklet_global_scope.h"
#include "third_party/blink/renderer/modules/modules_export.h"

namespace blink {

class WorkletGlobalScope;

// Mediates between a worklet-thread bound PaintWorkletGlobalScope and its
// associated dispatchers. A PaintWorkletProxyClient is associated with a single
// global scope and one dispatcher to the compositor thread.
//
// This is constructed on the main thread but it is used in the worklet backing
// thread.
//
// TODO(smcgruer): Add the dispatcher logic.
class MODULES_EXPORT PaintWorkletProxyClient
    : public GarbageCollectedFinalized<PaintWorkletProxyClient>,
      public Supplement<WorkerClients> {
  USING_GARBAGE_COLLECTED_MIXIN(PaintWorkletProxyClient);
  DISALLOW_COPY_AND_ASSIGN(PaintWorkletProxyClient);

 public:
  static const char kSupplementName[];

  static PaintWorkletProxyClient* Create();
  virtual ~PaintWorkletProxyClient() = default;

  void Trace(blink::Visitor*) override;

  virtual void SetGlobalScope(WorkletGlobalScope*);
  void Dispose();

  static PaintWorkletProxyClient* From(WorkerClients*);

 protected:
  PaintWorkletProxyClient();

 private:
  CrossThreadPersistent<PaintWorkletGlobalScope> global_scope_;

  enum RunState { kUninitialized, kWorking, kDisposed } state_;
};

void MODULES_EXPORT ProvidePaintWorkletProxyClientTo(WorkerClients*,
                                                     PaintWorkletProxyClient*);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_CSSPAINT_PAINT_WORKLET_PROXY_CLIENT_H_
