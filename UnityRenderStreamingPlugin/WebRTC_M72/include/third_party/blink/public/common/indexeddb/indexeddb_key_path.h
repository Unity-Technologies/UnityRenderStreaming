// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_COMMON_INDEXEDDB_INDEXEDDB_KEY_PATH_H_
#define THIRD_PARTY_BLINK_PUBLIC_COMMON_INDEXEDDB_INDEXEDDB_KEY_PATH_H_

#include <string>
#include <vector>

#include "base/logging.h"
#include "base/strings/string16.h"
#include "third_party/blink/public/common/common_export.h"
#include "third_party/blink/public/common/indexeddb/web_idb_types.h"

namespace blink {

class BLINK_COMMON_EXPORT IndexedDBKeyPath {
 public:
  IndexedDBKeyPath();  // Defaults to blink::WebIDBKeyPathTypeNull.
  explicit IndexedDBKeyPath(const base::string16&);
  explicit IndexedDBKeyPath(const std::vector<base::string16>&);
  IndexedDBKeyPath(const IndexedDBKeyPath& other);
  IndexedDBKeyPath(IndexedDBKeyPath&& other);
  ~IndexedDBKeyPath();
  IndexedDBKeyPath& operator=(const IndexedDBKeyPath& other);
  IndexedDBKeyPath& operator=(IndexedDBKeyPath&& other);

  bool IsNull() const { return type_ == blink::kWebIDBKeyPathTypeNull; }
  bool operator==(const IndexedDBKeyPath& other) const;

  blink::WebIDBKeyPathType type() const { return type_; }
  const std::vector<base::string16>& array() const;
  const base::string16& string() const;

 private:
  blink::WebIDBKeyPathType type_;
  base::string16 string_;
  std::vector<base::string16> array_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_COMMON_INDEXEDDB_INDEXEDDB_KEY_PATH_H_
