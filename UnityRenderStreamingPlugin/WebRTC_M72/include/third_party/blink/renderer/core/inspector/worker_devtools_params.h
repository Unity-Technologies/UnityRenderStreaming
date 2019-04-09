// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_INSPECTOR_WORKER_DEVTOOLS_PARAMS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_INSPECTOR_WORKER_DEVTOOLS_PARAMS_H_

#include "base/unguessable_token.h"
#include "third_party/blink/public/web/devtools_agent.mojom-blink.h"

namespace blink {

struct CORE_EXPORT WorkerDevToolsParams {
  mojom::blink::DevToolsAgentRequest agent_request;
  mojom::blink::DevToolsAgentHostPtrInfo agent_host_ptr_info;
  bool wait_for_debugger = false;
  base::UnguessableToken devtools_worker_token;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_INSPECTOR_WORKER_DEVTOOLS_PARAMS_H_
