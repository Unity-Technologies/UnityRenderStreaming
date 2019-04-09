// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_RESOURCE_LINK_FETCH_RESOURCE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_RESOURCE_LINK_FETCH_RESOURCE_H_

#include "third_party/blink/renderer/platform/loader/fetch/resource.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource_client.h"

namespace blink {

class FetchParameters;
class ResourceFetcher;

class LinkFetchResource final : public Resource {
 public:
  static Resource* Fetch(ResourceType, FetchParameters&, ResourceFetcher*);

  LinkFetchResource(const ResourceRequest&,
                    ResourceType,
                    const ResourceLoaderOptions&);
  ~LinkFetchResource() override;

 private:
  class LinkResourceFactory : public NonTextResourceFactory {
   public:
    explicit LinkResourceFactory(ResourceType type)
        : NonTextResourceFactory(type) {}

    Resource* Create(const ResourceRequest& request,
                     const ResourceLoaderOptions& options) const override {
      return MakeGarbageCollected<LinkFetchResource>(request, GetType(),
                                                     options);
    }
  };
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_RESOURCE_LINK_FETCH_RESOURCE_H_
