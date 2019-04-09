// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TESTING_LIBFUZZER_FUZZERS_SKIA_PATH_COMMON_H_
#define TESTING_LIBFUZZER_FUZZERS_SKIA_PATH_COMMON_H_

#include "third_party/skia/include/core/SkPath.h"

template <typename T>
static bool read(const uint8_t** data, size_t* size, T* value) {
  if (*size < sizeof(T))
    return false;

  *value = *reinterpret_cast<const T*>(*data);
  *data += sizeof(T);
  *size -= sizeof(T);
  return true;
}

void BuildPath(const uint8_t** data,
               size_t* size,
               SkPath* path,
               int last_verb);

#endif  // TESTING_LIBFUZZER_FUZZERS_SKIA_PATH_COMMON_H_
