// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_CSSPAINT_PAINT_WORKLET_GLOBAL_SCOPE_PROXY_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_CSSPAINT_PAINT_WORKLET_GLOBAL_SCOPE_PROXY_H_

#include "third_party/blink/renderer/core/workers/worklet_global_scope_proxy.h"

#include "base/single_thread_task_runner.h"
#include "third_party/blink/renderer/core/workers/main_thread_worklet_reporting_proxy.h"
#include "third_party/blink/renderer/modules/csspaint/paint_worklet_global_scope.h"
#include "third_party/blink/renderer/modules/modules_export.h"

namespace blink {

class CSSPaintDefinition;
class LocalFrame;
class WorkletModuleResponsesMap;

// A proxy for PaintWorklet to talk to PaintWorkletGlobalScope.
class MODULES_EXPORT PaintWorkletGlobalScopeProxy
    : public GarbageCollectedFinalized<PaintWorkletGlobalScopeProxy>,
      public WorkletGlobalScopeProxy {
  USING_GARBAGE_COLLECTED_MIXIN(PaintWorkletGlobalScopeProxy);

 public:
  static PaintWorkletGlobalScopeProxy* From(WorkletGlobalScopeProxy*);

  PaintWorkletGlobalScopeProxy(LocalFrame*,
                               WorkletModuleResponsesMap*,
                               PaintWorkletPendingGeneratorRegistry*,
                               size_t global_scope_number);
  ~PaintWorkletGlobalScopeProxy() override = default;

  // Implements WorkletGlobalScopeProxy.
  void FetchAndInvokeScript(
      const KURL& module_url_record,
      network::mojom::FetchCredentialsMode,
      FetchClientSettingsObjectSnapshot* outside_settings_object,
      scoped_refptr<base::SingleThreadTaskRunner> outside_settings_task_runner,
      WorkletPendingTasks*) override;
  void WorkletObjectDestroyed() override;
  void TerminateWorkletGlobalScope() override;

  CSSPaintDefinition* FindDefinition(const String& name);

  PaintWorkletGlobalScope* global_scope() const { return global_scope_.Get(); }

  void Trace(blink::Visitor*) override;

 private:
  std::unique_ptr<MainThreadWorkletReportingProxy> reporting_proxy_;
  Member<PaintWorkletGlobalScope> global_scope_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_CSSPAINT_PAINT_WORKLET_GLOBAL_SCOPE_PROXY_H_
