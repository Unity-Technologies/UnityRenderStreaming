// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_MODULESCRIPT_MODULE_SCRIPT_LOADER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_MODULESCRIPT_MODULE_SCRIPT_LOADER_H_

#include "base/macros.h"
#include "third_party/blink/public/platform/web_url_request.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/loader/modulescript/module_script_creation_params.h"
#include "third_party/blink/renderer/core/loader/modulescript/module_script_fetch_request.h"
#include "third_party/blink/renderer/core/loader/modulescript/module_script_fetcher.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"

namespace blink {

class FetchClientSettingsObjectSnapshot;
class Modulator;
class ModuleScript;
class ModuleScriptLoaderClient;
class ModuleScriptLoaderRegistry;
enum class ModuleGraphLevel;

// ModuleScriptLoader is responsible for loading a new single ModuleScript.
//
// ModuleScriptLoader constructs FetchParameters and asks ModuleScriptFetcher
// to fetch a script with the parameters. Then, it returns its client a compiled
// ModuleScript.
//
// ModuleScriptLoader(s) should only be used via Modulator and its ModuleMap.
class CORE_EXPORT ModuleScriptLoader final
    : public GarbageCollectedFinalized<ModuleScriptLoader>,
      public ModuleScriptFetcher::Client {
  USING_GARBAGE_COLLECTED_MIXIN(ModuleScriptLoader);

  enum class State {
    kInitial,
    // FetchParameters is being processed, and ModuleScriptLoader hasn't
    // notifyFinished().
    kFetching,
    // Finished successfully or w/ error.
    kFinished,
  };

 public:
  ModuleScriptLoader(Modulator*,
                     const ScriptFetchOptions&,
                     ModuleScriptLoaderRegistry*,
                     ModuleScriptLoaderClient*);
  ~ModuleScriptLoader();

  static void Fetch(
      const ModuleScriptFetchRequest&,
      FetchClientSettingsObjectSnapshot* fetch_client_settings_object,
      ModuleGraphLevel,
      Modulator* module_map_settings_object,
      ModuleScriptCustomFetchType,
      ModuleScriptLoaderRegistry*,
      ModuleScriptLoaderClient*);

  // Implements ModuleScriptFetcher::Client.
  void NotifyFetchFinished(
      const base::Optional<ModuleScriptCreationParams>&,
      const HeapVector<Member<ConsoleMessage>>& error_messages) override;

  bool IsInitialState() const { return state_ == State::kInitial; }
  bool HasFinished() const { return state_ == State::kFinished; }

  void Trace(blink::Visitor*) override;

 private:
  void FetchInternal(
      const ModuleScriptFetchRequest&,
      FetchClientSettingsObjectSnapshot* fetch_client_settings_object,
      ModuleGraphLevel,
      ModuleScriptCustomFetchType);

  void AdvanceState(State new_state);
#if DCHECK_IS_ON()
  static const char* StateToString(State);
#endif

  Member<Modulator> modulator_;
  State state_ = State::kInitial;
  const ScriptFetchOptions options_;
  Member<ModuleScript> module_script_;
  Member<ModuleScriptLoaderRegistry> registry_;
  Member<ModuleScriptLoaderClient> client_;
  Member<ModuleScriptFetcher> module_fetcher_;
#if DCHECK_IS_ON()
  KURL url_;
#endif

  DISALLOW_COPY_AND_ASSIGN(ModuleScriptLoader);
};

}  // namespace blink

#endif
