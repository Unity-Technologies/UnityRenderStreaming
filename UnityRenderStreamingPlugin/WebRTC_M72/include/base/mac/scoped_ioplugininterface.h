// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MAC_SCOPED_IOPLUGININTERFACE_H_
#define BASE_MAC_SCOPED_IOPLUGININTERFACE_H_

#include <IOKit/IOKitLib.h>

#include "base/mac/scoped_typeref.h"

namespace base {
namespace mac {

namespace internal {

template <typename T>
struct ScopedIOPluginInterfaceTraits {
  static T InvalidValue() { return nullptr; }
  static T Retain(T t) {
    (*t)->AddRef(t);
    return t;
  }
  static void Release(T t) { (*t)->Release(t); }
};

}  // namespace internal

// Just like ScopedCFTypeRef but for IOCFPlugInInterface and friends
// (IOUSBInterfaceStruct and IOUSBDeviceStruct320 in particular).
template <typename T>
using ScopedIOPluginInterface =
    ScopedTypeRef<T**, internal::ScopedIOPluginInterfaceTraits<T**>>;

}  // namespace mac
}  // namespace base

#endif  // BASE_MAC_SCOPED_IOPLUGININTERFACE_H_
