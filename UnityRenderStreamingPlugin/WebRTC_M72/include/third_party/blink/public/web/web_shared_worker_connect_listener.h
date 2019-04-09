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

#ifndef THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_SHARED_WORKER_CONNECT_LISTENER_H_
#define THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_SHARED_WORKER_CONNECT_LISTENER_H_

#include "third_party/blink/public/mojom/shared_worker/shared_worker_creation_context_type.mojom-shared.h"
#include "third_party/blink/public/platform/web_feature.mojom-shared.h"

namespace blink {

// This is the callback interface passed from blink to the embedder.
class WebSharedWorkerConnectListener {
 public:
  virtual ~WebSharedWorkerConnectListener() = default;

  // Called when a worker is created.
  virtual void WorkerCreated(mojom::SharedWorkerCreationContextType) = 0;

  // Called when worker script load fails.
  virtual void ScriptLoadFailed() = 0;

  // Called when a connection is established.
  virtual void Connected() = 0;

  // Called when some API to be recorded in UseCounter is called on the worker
  // global scope.
  virtual void CountFeature(mojom::WebFeature) = 0;
};

}  // namespace blink

#endif
