// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MAC_SCOPED_LAUNCH_DATA_H_
#define BASE_MAC_SCOPED_LAUNCH_DATA_H_

#include <launch.h>

#include "base/scoped_generic.h"

namespace base {
namespace mac {

namespace internal {

struct ScopedLaunchDataTraits {
  static launch_data_t InvalidValue() { return nullptr; }
  static void Free(launch_data_t ldt) { launch_data_free(ldt); }
};

}  // namespace internal

// Just like std::unique_ptr<> but for launch_data_t.
using ScopedLaunchData =
    ScopedGeneric<launch_data_t, internal::ScopedLaunchDataTraits>;

}  // namespace mac
}  // namespace base

#endif  // BASE_MAC_SCOPED_LAUNCH_DATA_H_
