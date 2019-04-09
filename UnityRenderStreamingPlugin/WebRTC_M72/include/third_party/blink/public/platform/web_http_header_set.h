// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_HTTP_HEADER_SET_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_HTTP_HEADER_SET_H_

#include <set>
#include <string>
#include "base/strings/string_util.h"

namespace blink {

struct CompareIgnoreCase {
  bool operator()(const std::string& left, const std::string& right) const {
    return base::CompareCaseInsensitiveASCII(left, right) < 0;
  }
};

using WebHTTPHeaderSet = std::set<std::string, CompareIgnoreCase>;

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_HTTP_HEADER_SET_H_
