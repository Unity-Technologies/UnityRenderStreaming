// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_UNRAR_SRC_UNRAR_WRAPPER_H_
#define THIRD_PARTY_UNRAR_SRC_UNRAR_WRAPPER_H_

#include "rar.hpp"

namespace third_party_unrar {
using ::Archive;
static const int kUnrarEndarcHead = ::HEAD_ENDARC;
}  // namespace third_party_unrar

#endif  // THIRD_PARTY_UNRAR_SRC_UNRAR_WRAPPER_H_
