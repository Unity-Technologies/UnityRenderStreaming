// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_CUSTOM_ELEMENT_INTERNALS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_CUSTOM_ELEMENT_INTERNALS_H_

#include "third_party/blink/renderer/core/html/forms/listed_element.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/wtf/casting.h"

namespace blink {

class HTMLElement;

class ElementInternals : public ScriptWrappable, public ListedElement {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(ElementInternals);

 public:
  ElementInternals(HTMLElement& target);
  void Trace(Visitor* visitor) override;

  HTMLElement& Target() const { return *target_; }
  void DidUpgrade();

  // IDL attributes/operations
  HTMLFormElement* form() const;

 private:
  // ListedElement overrides:
  bool IsFormControlElement() const override;
  bool IsElementInternals() const override;
  bool IsEnumeratable() const override;

  Member<HTMLElement> target_;

  DISALLOW_COPY_AND_ASSIGN(ElementInternals);
};

template <>
struct DowncastTraits<ElementInternals> {
  static bool AllowFrom(const ListedElement& listed_element) {
    return listed_element.IsElementInternals();
  }
};

}  // namespace blink
#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_CUSTOM_ELEMENT_INTERNALS_H_
