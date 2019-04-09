// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_MODULESCRIPT_DOCUMENT_MODULE_SCRIPT_FETCHER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_MODULESCRIPT_DOCUMENT_MODULE_SCRIPT_FETCHER_H_

#include "third_party/blink/renderer/core/loader/modulescript/module_script_fetcher.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource_fetcher.h"

namespace blink {

// DocumentModuleScriptFetcher is an implmenetation of ModuleScriptFetcher
// interface used for <script type='module'> on Document.
// TODO(nhiroki): This class is also used for non-custom module script fetch on
// workers. We should rename this to something like ModuleScriptFetcherImpl that
// doesn't relate to Document.
class CORE_EXPORT DocumentModuleScriptFetcher final
    : public GarbageCollectedFinalized<DocumentModuleScriptFetcher>,
      public ModuleScriptFetcher {
  USING_GARBAGE_COLLECTED_MIXIN(DocumentModuleScriptFetcher);

 public:
  explicit DocumentModuleScriptFetcher(ResourceFetcher*);
  ~DocumentModuleScriptFetcher() override = default;

  // Implements ModuleScriptFetcher.
  void Fetch(FetchParameters&, ModuleGraphLevel, Client*) override;

  // Implements ResourceClient
  void NotifyFinished(Resource*) override;
  String DebugName() const override { return "DocumentModuleScriptFetcher"; }

  void Trace(blink::Visitor*) override;

 private:
  bool FetchIfLayeredAPI(FetchParameters&);

  const Member<ResourceFetcher> fetcher_;
  Member<Client> client_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_MODULESCRIPT_DOCUMENT_MODULE_SCRIPT_FETCHER_H_
