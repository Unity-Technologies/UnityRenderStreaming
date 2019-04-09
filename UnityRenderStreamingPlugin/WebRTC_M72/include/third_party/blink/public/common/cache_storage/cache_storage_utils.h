// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_COMMON_CACHE_STORAGE_CACHE_STORAGE_UTILS_H_
#define THIRD_PARTY_BLINK_PUBLIC_COMMON_CACHE_STORAGE_CACHE_STORAGE_UTILS_H_

#include "third_party/blink/public/common/common_export.h"

namespace blink {
namespace cache_storage {

// Define the base message used when a batch operation, like addAll(),
// has duplicate requests in its argument list.  This is populated by
// content on the browser side.  The blink code in the renderer looks
// for the message to record a UseCounter event.
// TODO(crbug.com/877737): Remove this once the cache.addAll()
// duplicate rejection finally ships.
BLINK_COMMON_EXPORT extern const char kDuplicateOperationBaseMessage[];

}  // namespace cache_storage
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_COMMON_CACHE_STORAGE_CACHE_STORAGE_UTILS_H_
