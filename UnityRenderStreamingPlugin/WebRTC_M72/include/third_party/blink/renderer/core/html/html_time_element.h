// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_TIME_ELEMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_TIME_ELEMENT_H_

#include "third_party/blink/renderer/core/html/html_element.h"

namespace blink {

class CORE_EXPORT HTMLTimeElement final : public HTMLElement {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static HTMLTimeElement* Create(Document&);

  HTMLTimeElement(Document&);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_TIME_ELEMENT_H_
