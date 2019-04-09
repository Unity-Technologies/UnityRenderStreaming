// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SCRIPT_MODULE_MAP_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SCRIPT_MODULE_MAP_H_

#include "base/macros.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/bindings/name_client.h"
#include "third_party/blink/renderer/platform/bindings/trace_wrapper_member.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/weborigin/kurl_hash.h"
#include "third_party/blink/renderer/platform/wtf/hash_map.h"

namespace blink {

class FetchClientSettingsObjectSnapshot;
class Modulator;
class ModuleScript;
class ModuleScriptFetchRequest;
class ModuleScriptLoaderRegistry;
class SingleModuleClient;
enum class ModuleGraphLevel;
enum class ModuleScriptCustomFetchType;

// A ModuleMap implements "module map" spec.
// https://html.spec.whatwg.org/multipage/webappapis.html#module-map
class CORE_EXPORT ModuleMap final : public GarbageCollected<ModuleMap>,
                                    public NameClient {
  class Entry;

 public:
  static ModuleMap* Create(Modulator* modulator) {
    return MakeGarbageCollected<ModuleMap>(modulator);
  }

  explicit ModuleMap(Modulator*);

  void Trace(blink::Visitor*);
  const char* NameInHeapSnapshot() const override { return "ModuleMap"; }

  // https://html.spec.whatwg.org/multipage/webappapis.html#fetch-a-single-module-script
  void FetchSingleModuleScript(
      const ModuleScriptFetchRequest&,
      FetchClientSettingsObjectSnapshot* fetch_client_settings_object,
      ModuleGraphLevel,
      ModuleScriptCustomFetchType,
      SingleModuleClient*);

  // Synchronously get the ModuleScript for a given URL.
  // If the URL wasn't fetched, or is currently being fetched, this returns a
  // nullptr.
  ModuleScript* GetFetchedModuleScript(const KURL&) const;

  Modulator* GetModulator() { return modulator_; }

 private:
  using MapImpl = HeapHashMap<KURL, TraceWrapperMember<Entry>>;

  // A module map is a map of absolute URLs to map entry.
  MapImpl map_;

  Member<Modulator> modulator_;
  Member<ModuleScriptLoaderRegistry> loader_registry_;
  DISALLOW_COPY_AND_ASSIGN(ModuleMap);
};

}  // namespace blink

#endif
