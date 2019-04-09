// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_SERVICE_WORKER_SERVICE_WORKER_INSTALLED_SCRIPTS_MANAGER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_SERVICE_WORKER_SERVICE_WORKER_INSTALLED_SCRIPTS_MANAGER_H_

#include "third_party/blink/public/mojom/service_worker/service_worker_installed_scripts_manager.mojom-blink.h"
#include "third_party/blink/renderer/core/workers/installed_scripts_manager.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/modules/service_worker/thread_safe_script_container.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/weborigin/kurl_hash.h"
#include "third_party/blink/renderer/platform/wtf/hash_set.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

// ServiceWorkerInstalledScriptsManager provides the main script and imported
// scripts of an installed service worker. The scripts are streamed from the
// browser process in parallel with worker thread initialization.
class MODULES_EXPORT ServiceWorkerInstalledScriptsManager
    : public InstalledScriptsManager {
 public:
  ServiceWorkerInstalledScriptsManager(
      const Vector<KURL>& installed_urls,
      mojom::blink::ServiceWorkerInstalledScriptsManagerRequest,
      mojom::blink::ServiceWorkerInstalledScriptsManagerHostPtrInfo,
      scoped_refptr<base::SingleThreadTaskRunner> io_task_runner);
  virtual ~ServiceWorkerInstalledScriptsManager() = default;

  // InstalledScriptsManager implementation.
  bool IsScriptInstalled(const KURL& script_url) const override;
  std::unique_ptr<ScriptData> GetScriptData(const KURL& script_url) override;

 private:
  friend class ServiceWorkerInstalledScriptsManagerTest;

  std::unique_ptr<ThreadSafeScriptContainer::RawScriptData> GetRawScriptData(
      const KURL& script_url);

  HashSet<KURL> installed_urls_;
  scoped_refptr<ThreadSafeScriptContainer> script_container_;
  scoped_refptr<
      mojom::blink::ThreadSafeServiceWorkerInstalledScriptsManagerHostPtr>
      manager_host_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_SERVICE_WORKER_SERVICE_WORKER_INSTALLED_SCRIPTS_MANAGER_H_
