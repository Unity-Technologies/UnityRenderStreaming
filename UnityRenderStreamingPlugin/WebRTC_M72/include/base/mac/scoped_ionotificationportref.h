// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MAC_SCOPED_IONOTIFICATIONPORTREF_H_
#define BASE_MAC_SCOPED_IONOTIFICATIONPORTREF_H_

#include <IOKit/IOKitLib.h>

#include "base/scoped_generic.h"

namespace base {
namespace mac {

namespace internal {

struct ScopedIONotificationPortRefTraits {
  static IONotificationPortRef InvalidValue() { return nullptr; }
  static void Free(IONotificationPortRef object) {
    IONotificationPortDestroy(object);
  }
};

}  // namepsace internal

using ScopedIONotificationPortRef =
    ScopedGeneric<IONotificationPortRef,
                  internal::ScopedIONotificationPortRefTraits>;

}  // namespace mac
}  // namespace base

#endif  // BASE_MAC_SCOPED_IONOTIFICATIONPORTREF_H_
