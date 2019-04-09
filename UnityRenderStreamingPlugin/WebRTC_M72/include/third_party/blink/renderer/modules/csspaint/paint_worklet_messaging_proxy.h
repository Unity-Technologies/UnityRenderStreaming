// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_CSSPAINT_PAINT_WORKLET_MESSAGING_PROXY_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_CSSPAINT_PAINT_WORKLET_MESSAGING_PROXY_H_

#include <memory>
#include "third_party/blink/renderer/core/workers/threaded_worklet_messaging_proxy.h"

namespace blink {

class ExecutionContext;
class WorkerThread;

// Acts as a proxy for worklet-thread bound PaintWorkletGlobalScopes. The logic
// to actually proxy an off thread global scope is implemented in the parent.
// The main contribution of this class is to create an appropriate worklet
// thread type as part of the the worklet initialization process.
class PaintWorkletMessagingProxy final : public ThreadedWorkletMessagingProxy {
 public:
  explicit PaintWorkletMessagingProxy(ExecutionContext*);

  void Trace(blink::Visitor*) override;

 private:
  ~PaintWorkletMessagingProxy() override;

  std::unique_ptr<WorkerThread> CreateWorkerThread() override;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_CSSPAINT_OFF_THREAD_PAINT_WORKLET_MESSAGING_PROXY_H_
