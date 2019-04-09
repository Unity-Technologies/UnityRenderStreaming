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

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_INDEXEDDB_IDB_KEY_PATH_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_INDEXEDDB_IDB_KEY_PATH_H_

#include "third_party/blink/public/platform/modules/indexeddb/web_idb_key_path.h"
#include "third_party/blink/renderer/bindings/core/v8/string_or_string_sequence.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

enum IDBKeyPathParseError {
  kIDBKeyPathParseErrorNone,
  kIDBKeyPathParseErrorIdentifier,
};

MODULES_EXPORT void IDBParseKeyPath(const String&,
                                    Vector<String>&,
                                    IDBKeyPathParseError&);

class MODULES_EXPORT IDBKeyPath {
  DISALLOW_NEW();

 public:
  IDBKeyPath() : type_(kNullType) {}
  explicit IDBKeyPath(const String&);
  explicit IDBKeyPath(const Vector<String>& array);
  explicit IDBKeyPath(const StringOrStringSequence& key_path);
  IDBKeyPath(const WebIDBKeyPath&);

  operator WebIDBKeyPath() const;

  enum Type { kNullType = 0, kStringType, kArrayType };

  Type GetType() const { return type_; }

  const Vector<String>& Array() const {
    DCHECK_EQ(type_, kArrayType);
    return array_;
  }

  const String& GetString() const {
    DCHECK_EQ(type_, kStringType);
    return string_;
  }

  bool IsNull() const { return type_ == kNullType; }
  bool IsValid() const;
  bool operator==(const IDBKeyPath& other) const;

 private:
  Type type_;
  class String string_;
  Vector<class String> array_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_INDEXEDDB_IDB_KEY_PATH_H_
