// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SCRIPT_DOCUMENT_MODULATOR_IMPL_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SCRIPT_DOCUMENT_MODULATOR_IMPL_H_

#include "third_party/blink/renderer/core/script/modulator_impl_base.h"

#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class ModuleScriptFetcher;
class ResourceFetcher;
class ScriptState;

// DocumentModulatorImpl is the Modulator implementation used in main documents
// (that means, not worker nor worklets). Module operations depending on the
// Document context should be implemented in this class, not in
// ModulatorImplBase.
class DocumentModulatorImpl final : public ModulatorImplBase {
 public:
  static ModulatorImplBase* Create(ScriptState*, ResourceFetcher*);

  DocumentModulatorImpl(ScriptState*, ResourceFetcher*);

  // Implements Modulator.
  ModuleScriptFetcher* CreateModuleScriptFetcher(
      ModuleScriptCustomFetchType) override;

  void Trace(blink::Visitor*) override;

 private:
  // Implements ModulatorImplBase.
  bool IsDynamicImportForbidden(String* reason) override;

  Member<ResourceFetcher> fetcher_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SCRIPT_DOCUMENT_MODULATOR_IMPL_H_
