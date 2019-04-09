// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_COMMON_INDEXEDDB_INDEXEDDB_KEY_H_
#define THIRD_PARTY_BLINK_PUBLIC_COMMON_INDEXEDDB_INDEXEDDB_KEY_H_

#include <stddef.h>

#include <string>
#include <vector>

#include "base/logging.h"
#include "base/strings/string16.h"
#include "third_party/blink/public/common/common_export.h"
#include "third_party/blink/public/common/indexeddb/web_idb_types.h"

namespace blink {

class BLINK_COMMON_EXPORT IndexedDBKey {
 public:
  typedef std::vector<IndexedDBKey> KeyArray;

  IndexedDBKey();  // Defaults to blink::WebIDBKeyTypeInvalid.
  explicit IndexedDBKey(blink::WebIDBKeyType);  // must be Null or Invalid
  explicit IndexedDBKey(KeyArray array);
  explicit IndexedDBKey(std::string binary);
  explicit IndexedDBKey(base::string16 string);
  IndexedDBKey(double number,
               blink::WebIDBKeyType type);  // must be date or number
  IndexedDBKey(const IndexedDBKey& other);
  ~IndexedDBKey();
  IndexedDBKey& operator=(const IndexedDBKey& other);

  bool IsValid() const;

  bool IsLessThan(const IndexedDBKey& other) const;
  bool Equals(const IndexedDBKey& other) const;

  blink::WebIDBKeyType type() const { return type_; }
  const std::vector<IndexedDBKey>& array() const {
    DCHECK_EQ(type_, blink::kWebIDBKeyTypeArray);
    return array_;
  }
  const std::string& binary() const {
    DCHECK_EQ(type_, blink::kWebIDBKeyTypeBinary);
    return binary_;
  }
  const base::string16& string() const {
    DCHECK_EQ(type_, blink::kWebIDBKeyTypeString);
    return string_;
  }
  double date() const {
    DCHECK_EQ(type_, blink::kWebIDBKeyTypeDate);
    return number_;
  }
  double number() const {
    DCHECK_EQ(type_, blink::kWebIDBKeyTypeNumber);
    return number_;
  }

  size_t size_estimate() const { return size_estimate_; }

 private:
  int CompareTo(const IndexedDBKey& other) const;

  blink::WebIDBKeyType type_;
  std::vector<IndexedDBKey> array_;
  std::string binary_;
  base::string16 string_;
  double number_ = 0;

  size_t size_estimate_;
};

// An index id, and corresponding set of keys to insert.
using IndexedDBIndexKeys = std::pair<int64_t, std::vector<IndexedDBKey>>;

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_COMMON_INDEXEDDB_INDEXEDDB_KEY_H_
