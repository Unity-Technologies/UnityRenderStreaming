// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_SERVICE_WORKER_SERVICE_WORKER_CONTENT_SETTINGS_PROXY_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_SERVICE_WORKER_SERVICE_WORKER_CONTENT_SETTINGS_PROXY_H_

#include "third_party/blink/public/platform/web_content_settings_client.h"
#include "third_party/blink/public/platform/web_security_origin.h"
#include "third_party/blink/public/web/worker_content_settings_proxy.mojom-blink.h"
#include "third_party/blink/renderer/platform/weborigin/security_origin.h"

namespace blink {

// Provides the content settings information from browser process.
// This proxy is created and destroyed on the main thread and used on the
// worker thread.
class ServiceWorkerContentSettingsProxy final
    : public blink::WebContentSettingsClient {
 public:
  explicit ServiceWorkerContentSettingsProxy(
      mojom::blink::WorkerContentSettingsProxyPtrInfo host_info);
  ~ServiceWorkerContentSettingsProxy() override;

  void SetSecurityOrigin(scoped_refptr<const blink::SecurityOrigin>);

  // WebContentSettingsClient overrides.
  // Asks the browser process about the settings.
  // Blocks until the response arrives.
  bool RequestFileSystemAccessSync() override;
  bool AllowIndexedDB(const WebSecurityOrigin&) override;

 private:
  // To ensure the returned pointer is destructed on the same thread
  // that it was constructed on, this uses ThreadSpecific.
  mojom::blink::WorkerContentSettingsProxyPtr& GetService();

  // This is set on the main thread at the ctor,
  // and moved to thread local storage on the worker thread
  // when GetService() is called for the first time.
  mojom::blink::WorkerContentSettingsProxyPtrInfo host_info_;

  DISALLOW_COPY_AND_ASSIGN(ServiceWorkerContentSettingsProxy);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_SERVICE_WORKER_SERVICE_WORKER_CONTENT_SETTINGS_PROXY_H_
