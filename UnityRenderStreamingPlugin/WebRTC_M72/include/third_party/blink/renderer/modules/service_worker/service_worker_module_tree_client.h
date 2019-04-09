// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_SERVICE_WORKER_SERVICE_WORKER_MODULE_TREE_CLIENT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_SERVICE_WORKER_SERVICE_WORKER_MODULE_TREE_CLIENT_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/script/modulator.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"

namespace blink {

class ModuleScript;

// This is an implementation of ModuleTreeClient for service workers that lives
// on the worker context's thread.
class ServiceWorkerModuleTreeClient final : public ModuleTreeClient {
 public:
  explicit ServiceWorkerModuleTreeClient(Modulator*);

  // Implements ModuleTreeClient.
  void NotifyModuleTreeLoadFinished(ModuleScript*) final;

  void Trace(blink::Visitor*) override;

 private:
  Member<Modulator> modulator_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_SERVICE_WORKER_SERVICE_WORKER_MODULE_TREE_CLIENT_H_
