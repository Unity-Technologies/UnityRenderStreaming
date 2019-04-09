// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_REL_LIST_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_REL_LIST_H_

#include "third_party/blink/renderer/core/dom/dom_token_list.h"

namespace blink {

class RelList final : public DOMTokenList {
 public:
  static RelList* Create(Element* element) { return new RelList(element); }

 private:
  explicit RelList(Element*);
  bool ValidateTokenValue(const AtomicString&, ExceptionState&) const override;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_REL_LIST_H_
