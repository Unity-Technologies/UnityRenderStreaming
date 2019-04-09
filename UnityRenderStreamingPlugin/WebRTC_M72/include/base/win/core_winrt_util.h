// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_WIN_CORE_WINRT_UTIL_H_
#define BASE_WIN_CORE_WINRT_UTIL_H_

#include <hstring.h>
#include <inspectable.h>
#include <roapi.h>
#include <windef.h>

#include "base/base_export.h"
#include "base/strings/string16.h"
#include "base/win/scoped_hstring.h"

namespace base {
namespace win {

// Provides access to Core WinRT functions which may not be available on
// Windows 7. Loads functions dynamically at runtime to prevent library
// dependencies.

BASE_EXPORT bool ResolveCoreWinRTDelayload();

// The following stubs are provided for when component build is enabled, in
// order to avoid the propagation of delay-loading CoreWinRT to other modules.

BASE_EXPORT HRESULT RoInitialize(RO_INIT_TYPE init_type);

BASE_EXPORT void RoUninitialize();

BASE_EXPORT HRESULT RoGetActivationFactory(HSTRING class_id,
                                           const IID& iid,
                                           void** out_factory);

BASE_EXPORT HRESULT RoActivateInstance(HSTRING class_id,
                                       IInspectable** instance);

// Retrieves an activation factory for the type specified.
template <typename InterfaceType, char16 const* runtime_class_id>
HRESULT GetActivationFactory(InterfaceType** factory) {
  ScopedHString class_id_hstring = ScopedHString::Create(runtime_class_id);
  if (!class_id_hstring.is_valid())
    return E_FAIL;

  return base::win::RoGetActivationFactory(class_id_hstring.get(),
                                           IID_PPV_ARGS(factory));
}

}  // namespace win
}  // namespace base

#endif  // BASE_WIN_CORE_WINRT_UTIL_H_
