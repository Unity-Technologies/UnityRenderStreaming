// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_DOCUMENT_SUBRESOURCE_FILTER_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_DOCUMENT_SUBRESOURCE_FILTER_H_

#include "third_party/blink/public/platform/modules/fetch/fetch_api_request.mojom-shared.h"

namespace blink {

class WebURL;

class WebDocumentSubresourceFilter {
 public:
  // This builder class is created on the main thread and passed to a worker
  // thread to create the subresource filter for the worker thread.
  class Builder {
   public:
    virtual ~Builder() = default;
    virtual std::unique_ptr<WebDocumentSubresourceFilter> Build() = 0;
  };

  enum LoadPolicy { kAllow, kDisallow, kWouldDisallow };

  virtual ~WebDocumentSubresourceFilter() = default;
  virtual LoadPolicy GetLoadPolicy(const WebURL& resource_url,
                                   mojom::RequestContextType) = 0;
  virtual LoadPolicy GetLoadPolicyForWebSocketConnect(const WebURL&) = 0;

  // Report that a resource loaded by the document (not a preload) was
  // disallowed.
  virtual void ReportDisallowedLoad() = 0;

  // Returns true if disallowed resource loads should be logged to the devtools
  // console.
  virtual bool ShouldLogToConsole() = 0;

  // Report that the resource request corresponding to |request_id|  was tagged
  // as an ad.
  virtual void ReportAdRequestId(int request_id) {}
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_DOCUMENT_SUBRESOURCE_FILTER_H_
