// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.+

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_INDEXEDDB_IDB_OBSERVATION_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_INDEXEDDB_IDB_OBSERVATION_H_

#include <memory>

#include "third_party/blink/public/common/indexeddb/web_idb_types.h"
#include "third_party/blink/public/mojom/indexeddb/indexeddb.mojom-blink.h"
#include "third_party/blink/renderer/bindings/core/v8/script_value.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class IDBAny;
class IDBKeyRange;
class ScriptState;
struct WebIDBObservation;

class IDBObservation final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static mojom::IDBOperationType StringToOperationType(const String&);

  // Consumes the WebIDBObservation.
  static IDBObservation* Create(WebIDBObservation, v8::Isolate*);

  IDBObservation(WebIDBObservation, v8::Isolate*);
  ~IDBObservation() override;

  void Trace(blink::Visitor*) override;

  // Implement the IDL
  ScriptValue key(ScriptState*);
  ScriptValue value(ScriptState*);
  const String& type() const;

 private:
  Member<IDBKeyRange> key_range_;
  Member<IDBAny> value_;
  const mojom::IDBOperationType operation_type_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_INDEXEDDB_IDB_OBSERVATION_H_
