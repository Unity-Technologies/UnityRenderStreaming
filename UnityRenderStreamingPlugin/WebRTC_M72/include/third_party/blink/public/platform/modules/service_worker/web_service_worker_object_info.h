// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_MODULES_SERVICE_WORKER_WEB_SERVICE_WORKER_OBJECT_INFO_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_MODULES_SERVICE_WORKER_WEB_SERVICE_WORKER_OBJECT_INFO_H_

#include "base/macros.h"
#include "mojo/public/cpp/bindings/scoped_interface_endpoint_handle.h"
#include "third_party/blink/public/mojom/service_worker/service_worker_state.mojom-shared.h"
#include "third_party/blink/public/platform/web_url.h"

namespace blink {

// This is to carry blink.mojom.ServiceWorkerObjectInfo data from //content
// across the boundary into Blink.
// TODO(crbug.com/879019): Remove this class once we make the following Mojo
// interfaces receive blink.mojom.ServiceWorkerObjectInfo directly inside Blink.
//  - content.mojom.ServiceWorker
//  - content.mojom.ServiceWorkerContainer
//
// As we're on the border line between non-Blink and Blink variants, we need
// to use mojo::ScopedInterfaceEndpointHandle to pass Mojo types.
struct WebServiceWorkerObjectInfo {
  WebServiceWorkerObjectInfo(int64_t version_id,
                             mojom::ServiceWorkerState state,
                             WebURL url,
                             mojo::ScopedInterfaceEndpointHandle host_ptr_info,
                             mojo::ScopedInterfaceEndpointHandle request)
      : version_id(version_id),
        state(state),
        url(std::move(url)),
        host_ptr_info(std::move(host_ptr_info)),
        request(std::move(request)) {}
  WebServiceWorkerObjectInfo(WebServiceWorkerObjectInfo&& other) = default;

  int64_t version_id;
  mojom::ServiceWorkerState state;
  WebURL url;
  // For blink::mojom::ServiceWorkerObjectHostAssociatedPtrInfo.
  mojo::ScopedInterfaceEndpointHandle host_ptr_info;
  // For blink::mojom::ServiceWorkerObjectAssociatedRequest.
  mojo::ScopedInterfaceEndpointHandle request;

  DISALLOW_COPY_AND_ASSIGN(WebServiceWorkerObjectInfo);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_MODULES_SERVICE_WORKER_WEB_SERVICE_WORKER_OBJECT_INFO_H_
