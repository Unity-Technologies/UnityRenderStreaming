/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_INDEXEDDB_IDB_INDEX_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_INDEXEDDB_IDB_INDEX_H_

#include "third_party/blink/public/common/indexeddb/web_idb_types.h"
#include "third_party/blink/public/mojom/indexeddb/indexeddb.mojom-blink.h"
#include "third_party/blink/renderer/modules/indexeddb/idb_cursor.h"
#include "third_party/blink/renderer/modules/indexeddb/idb_key_path.h"
#include "third_party/blink/renderer/modules/indexeddb/idb_key_range.h"
#include "third_party/blink/renderer/modules/indexeddb/idb_metadata.h"
#include "third_party/blink/renderer/modules/indexeddb/idb_request.h"
#include "third_party/blink/renderer/modules/indexeddb/web_idb_cursor.h"
#include "third_party/blink/renderer/modules/indexeddb/web_idb_database.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class ExceptionState;
class IDBObjectStore;

class IDBIndex final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static IDBIndex* Create(scoped_refptr<IDBIndexMetadata> metadata,
                          IDBObjectStore* object_store,
                          IDBTransaction* transaction) {
    return MakeGarbageCollected<IDBIndex>(std::move(metadata), object_store,
                                          transaction);
  }

  IDBIndex(scoped_refptr<IDBIndexMetadata>, IDBObjectStore*, IDBTransaction*);
  ~IDBIndex() override;

  void Trace(blink::Visitor*) override;

  // Implement the IDL
  const String& name() const { return Metadata().name; }
  void setName(const String& name, ExceptionState&);
  IDBObjectStore* objectStore() const { return object_store_.Get(); }
  ScriptValue keyPath(ScriptState*) const;
  bool unique() const { return Metadata().unique; }
  bool multiEntry() const { return Metadata().multi_entry; }

  IDBRequest* openCursor(ScriptState*,
                         const ScriptValue& key,
                         const String& direction,
                         ExceptionState&);
  IDBRequest* openKeyCursor(ScriptState*,
                            const ScriptValue& range,
                            const String& direction,
                            ExceptionState&);
  IDBRequest* count(ScriptState*, const ScriptValue& range, ExceptionState&);
  IDBRequest* get(ScriptState*, const ScriptValue& key, ExceptionState&);
  IDBRequest* getAll(ScriptState*, const ScriptValue& range, ExceptionState&);
  IDBRequest* getAll(ScriptState*,
                     const ScriptValue& range,
                     unsigned long max_count,
                     ExceptionState&);
  IDBRequest* getKey(ScriptState*, const ScriptValue& key, ExceptionState&);
  IDBRequest* getAllKeys(ScriptState*,
                         const ScriptValue& range,
                         ExceptionState&);
  IDBRequest* getAllKeys(ScriptState*,
                         const ScriptValue& range,
                         uint32_t max_count,
                         ExceptionState&);

  void MarkDeleted() {
    DCHECK(transaction_->IsVersionChange())
        << "Index deleted outside versionchange transaction.";
    deleted_ = true;
  }
  bool IsDeleted() const { return deleted_; }
  int64_t Id() const { return Metadata().id; }

  // True if this index was created in its associated transaction.
  // Only valid if the index's associated transaction is a versionchange.
  bool IsNewlyCreated(
      const IDBObjectStoreMetadata& old_object_store_metadata) const {
    DCHECK(transaction_->IsVersionChange());

    // Index IDs are allocated sequentially, so we can tell if an index was
    // created in this transaction by comparing its ID against the object
    // store's maximum index ID at the time when the transaction was started.
    return Id() > old_object_store_metadata.max_index_id;
  }

  void RevertMetadata(scoped_refptr<IDBIndexMetadata> old_metadata);

  // Used internally and by InspectorIndexedDBAgent:
  IDBRequest* openCursor(
      ScriptState*,
      IDBKeyRange*,
      mojom::IDBCursorDirection,
      IDBRequest::AsyncTraceState = IDBRequest::AsyncTraceState());

  WebIDBDatabase* BackendDB() const;

 private:
  const IDBIndexMetadata& Metadata() const { return *metadata_; }

  IDBRequest* GetInternal(ScriptState*,
                          const ScriptValue& key,
                          ExceptionState&,
                          bool key_only,
                          IDBRequest::AsyncTraceState metrics);
  IDBRequest* GetAllInternal(ScriptState*,
                             const ScriptValue& range,
                             unsigned long max_count,
                             ExceptionState&,
                             bool key_only,
                             IDBRequest::AsyncTraceState metrics);

  scoped_refptr<IDBIndexMetadata> metadata_;
  Member<IDBObjectStore> object_store_;
  Member<IDBTransaction> transaction_;
  bool deleted_ = false;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_INDEXEDDB_IDB_INDEX_H_
