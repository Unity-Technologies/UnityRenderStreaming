// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_WIN_REFERENCE_H_
#define BASE_WIN_REFERENCE_H_

#include <windows.foundation.collections.h>
#include <wrl/implements.h>

#include <type_traits>
#include <utility>

#include "base/macros.h"

namespace base {
namespace win {

// Implementation of the UWP's IReference interface.
template <typename T>
class Reference
    : public Microsoft::WRL::RuntimeClass<
          Microsoft::WRL::RuntimeClassFlags<
              Microsoft::WRL::WinRt | Microsoft::WRL::InhibitRoOriginateError>,
          ABI::Windows::Foundation::IReference<T>> {
 public:
  using AbiT = typename ABI::Windows::Foundation::Internal::GetAbiType<
      typename ABI::Windows::Foundation::IReference<T>::T_complex>::type;

  explicit Reference(const AbiT& value) : value_(value) {}
  explicit Reference(AbiT&& value) : value_(std::move(value)) {}

  // ABI::Windows::Foundation::IReference:
  IFACEMETHODIMP get_Value(AbiT* value) override {
    *value = value_;
    return S_OK;
  }

 private:
  ~Reference() = default;
  AbiT value_;

  DISALLOW_COPY_AND_ASSIGN(Reference);
};

}  // namespace win
}  // namespace base

#endif  // BASE_WIN_REFERENCE_H_
