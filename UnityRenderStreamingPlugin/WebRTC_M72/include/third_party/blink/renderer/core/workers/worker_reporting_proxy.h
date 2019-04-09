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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_WORKER_REPORTING_PROXY_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_WORKER_REPORTING_PROXY_H_

#include <memory>
#include "third_party/blink/renderer/bindings/core/v8/source_location.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/frame/web_feature_forward.h"
#include "third_party/blink/renderer/core/inspector/console_types.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

namespace blink {

class WorkerOrWorkletGlobalScope;

// APIs used by workers to report console and worker activity. Some functions
// are called only for classic scripts and some of others are called only for
// module scripts. They're annotated with [classic script only] or [module
// script only].
class CORE_EXPORT WorkerReportingProxy {
 public:
  virtual ~WorkerReportingProxy() = default;

  virtual void CountFeature(WebFeature) {}
  virtual void CountDeprecation(WebFeature) {}
  virtual void ReportException(const String& error_message,
                               std::unique_ptr<SourceLocation>,
                               int exception_id) {}
  virtual void ReportConsoleMessage(MessageSource,
                                    MessageLevel,
                                    const String& message,
                                    SourceLocation*) {}

  // Invoked when the new WorkerGlobalScope is created on
  // WorkerThread::InitializeOnWorkerThread.
  virtual void DidCreateWorkerGlobalScope(WorkerOrWorkletGlobalScope*) {}

  // Invoked when the WorkerGlobalScope is initialized on
  // WorkerThread::InitializeOnWorkerThread.
  virtual void DidInitializeWorkerContext() {}

  // Invoked when the worker's main script is loaded on
  // WorkerThread::InitializeOnWorkerThread(). Only invoked when the script was
  // loaded on the worker thread, i.e., via InstalledScriptsManager rather than
  // via ResourceLoader. Called before WillEvaluateClassicScript().
  virtual void DidLoadInstalledScript() {}

  // Invoked when it's failed to load the worker's main script from
  // InstalledScriptsManager.
  virtual void DidFailToLoadInstalledClassicScript() {}

  // Invoked on failure to fetch the worker's module script (either from network
  // or InstalledScriptsManager).
  virtual void DidFailToFetchModuleScript() {}

  // Invoked when the main classic script is about to be evaluated.
  virtual void WillEvaluateClassicScript(size_t script_size,
                                         size_t cached_metadata_size) {}

  // Invoked when an imported classic script is about to be evaluated.
  virtual void WillEvaluateImportedClassicScript(size_t script_size,
                                                 size_t cached_metadata_size) {}

  // Invoked when the worker's main module script is about to be evaluated.
  virtual void WillEvaluateModuleScript() {}

  // Invoked when the main classic script is evaluated. |success| is true if the
  // evaluation completed with no uncaught exception.
  virtual void DidEvaluateClassicScript(bool success) {}

  // Invoked when the worker's main module script is evaluated. |success| is
  // true if the evaluation completed with no uncaught exception.
  virtual void DidEvaluateModuleScript(bool success) {}

  // Invoked when close() is invoked on the worker context.
  virtual void DidCloseWorkerGlobalScope() {}

  // Invoked when the thread is about to be stopped and WorkerGlobalScope
  // is to be destructed. When this is called, it is guaranteed that
  // WorkerGlobalScope is still alive.
  virtual void WillDestroyWorkerGlobalScope() {}

  // Invoked when the thread is stopped and WorkerGlobalScope is being
  // destructed. This is the last method that is called on this interface.
  virtual void DidTerminateWorkerThread() {}
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_WORKER_REPORTING_PROXY_H_
