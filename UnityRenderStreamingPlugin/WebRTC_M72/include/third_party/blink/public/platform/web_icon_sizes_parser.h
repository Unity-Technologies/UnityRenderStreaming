// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_ICON_SIZES_PARSER_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_ICON_SIZES_PARSER_H_

#include "third_party/blink/public/platform/web_common.h"
#include "third_party/blink/public/platform/web_vector.h"

namespace blink {

class WebString;
struct WebSize;

// Helper class for parsing icon sizes. The spec is:
// https://html.spec.whatwg.org/multipage/semantics.html#attr-link-sizes
// TODO(zqzhang): merge with WebIconURL, and rename it "WebIcon"?
class WebIconSizesParser {
 public:
  BLINK_PLATFORM_EXPORT static WebVector<WebSize> ParseIconSizes(
      const WebString& sizes_string);
};

}  // namespace blink

#endif
