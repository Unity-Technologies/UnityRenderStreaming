// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_LIBADDRESSINPUT_CHROMIUM_ADDRESSINPUT_UTIL_H_
#define THIRD_PARTY_LIBADDRESSINPUT_CHROMIUM_ADDRESSINPUT_UTIL_H_

#include <map>

#include "third_party/libaddressinput/src/cpp/include/libaddressinput/address_field.h"
#include "third_party/libaddressinput/src/cpp/include/libaddressinput/address_problem.h"

namespace i18n {
namespace addressinput {
struct AddressData;
}
}

namespace autofill {
namespace addressinput {

// Returns true if |address_to_check| has all of its required fields.
bool HasAllRequiredFields(
    const ::i18n::addressinput::AddressData& address_to_check);

// Validates required fields in |address_to_check| without loading rules from
// the server. The |problems| parameter cannot be NULL. Does not take ownership
// of its parameters.
//
// See documentation of ::i18n::addressinput::AddressValidator::Validate() for
// description of |filter| and |problems|.
void ValidateRequiredFields(
    const ::i18n::addressinput::AddressData& address_to_check,
    const std::multimap< ::i18n::addressinput::AddressField,
                         ::i18n::addressinput::AddressProblem>* filter,
    std::multimap< ::i18n::addressinput::AddressField,
                   ::i18n::addressinput::AddressProblem>* problems);

}  // namespace addressinput
}  // namespace autofill

#endif  // THIRD_PARTY_LIBADDRESSINPUT_CHROMIUM_ADDRESSINPUT_UTIL_H_
