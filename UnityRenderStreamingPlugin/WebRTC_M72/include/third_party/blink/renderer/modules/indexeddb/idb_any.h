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

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_INDEXEDDB_IDB_ANY_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_INDEXEDDB_IDB_ANY_H_

#include <memory>

#include "third_party/blink/renderer/core/dom/dom_string_list.h"
#include "third_party/blink/renderer/modules/indexeddb/idb_key.h"
#include "third_party/blink/renderer/modules/indexeddb/idb_value.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class IDBCursor;
class IDBCursorWithValue;
class IDBDatabase;
class IDBIndex;
class IDBObjectStore;

// IDBAny is used for:
//  * source of IDBCursor (IDBObjectStore or IDBIndex)
//  * source of IDBRequest (IDBObjectStore, IDBIndex, IDBCursor, or null)
//  * result of IDBRequest (IDBDatabase, IDBCursor, DOMStringList, undefined,
//    integer, key or value)
//
// This allows for lazy conversion to script values (via IDBBindingUtilities),
// and avoids the need for many dedicated union types.

class MODULES_EXPORT IDBAny : public GarbageCollectedFinalized<IDBAny> {
 public:
  static IDBAny* CreateUndefined();
  static IDBAny* CreateNull();
  template <typename T>
  static IDBAny* Create(T* idb_object) {
    return MakeGarbageCollected<IDBAny>(idb_object);
  }
  static IDBAny* Create(DOMStringList* dom_string_list) {
    return MakeGarbageCollected<IDBAny>(dom_string_list);
  }
  static IDBAny* Create(int64_t value) {
    return MakeGarbageCollected<IDBAny>(value);
  }
  static IDBAny* Create(std::unique_ptr<IDBKey> key) {
    return MakeGarbageCollected<IDBAny>(std::move(key));
  }
  static IDBAny* Create(std::unique_ptr<IDBValue> value) {
    return MakeGarbageCollected<IDBAny>(std::move(value));
  }
  static IDBAny* Create(Vector<std::unique_ptr<IDBValue>> values) {
    return MakeGarbageCollected<IDBAny>(std::move(values));
  }

  enum Type {
    kUndefinedType = 0,
    kNullType,
    kDOMStringListType,
    kIDBCursorType,
    kIDBCursorWithValueType,
    kIDBDatabaseType,
    kIntegerType,
    kKeyType,
    kIDBValueType,
    kIDBValueArrayType
  };

  explicit IDBAny(Type);
  explicit IDBAny(DOMStringList*);
  explicit IDBAny(IDBCursor*);
  explicit IDBAny(IDBDatabase*);
  explicit IDBAny(std::unique_ptr<IDBKey>);
  explicit IDBAny(Vector<std::unique_ptr<IDBValue>>);
  explicit IDBAny(std::unique_ptr<IDBValue>);
  explicit IDBAny(int64_t);
  ~IDBAny();

  void Trace(blink::Visitor*);
  void ContextWillBeDestroyed();

  Type GetType() const { return type_; }
  // Use type() to figure out which one of these you're allowed to call.
  DOMStringList* DomStringList() const;
  IDBCursor* IdbCursor() const;
  IDBCursorWithValue* IdbCursorWithValue() const;
  IDBDatabase* IdbDatabase() const;
  IDBValue* Value() const;
  const Vector<std::unique_ptr<IDBValue>>& Values() const;
  int64_t Integer() const;
  const IDBKey* Key() const;

 private:
  const Type type_;

  // Only one of the following should ever be in use at any given time.
  const Member<DOMStringList> dom_string_list_;
  const Member<IDBCursor> idb_cursor_;
  const Member<IDBDatabase> idb_database_;
  const std::unique_ptr<IDBKey> idb_key_;
  const std::unique_ptr<IDBValue> idb_value_;
  const Vector<std::unique_ptr<IDBValue>> idb_values_;
  const int64_t integer_ = 0;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_INDEXEDDB_IDB_ANY_H_
