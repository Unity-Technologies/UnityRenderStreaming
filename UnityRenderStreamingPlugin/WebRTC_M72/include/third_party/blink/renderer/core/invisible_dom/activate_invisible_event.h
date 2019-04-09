// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_INVISIBLE_DOM_ACTIVATE_INVISIBLE_EVENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_INVISIBLE_DOM_ACTIVATE_INVISIBLE_EVENT_H_

#include "third_party/blink/renderer/core/dom/events/event.h"

namespace blink {

class Element;

class ActivateInvisibleEvent : public Event {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static ActivateInvisibleEvent* Create(Element* activated_element) {
    return new ActivateInvisibleEvent(activated_element);
  }

  Element* activatedElement() const { return activated_element_.Get(); }

  void SetActivatedElement(Element* activated_element) {
    activated_element_ = activated_element;
  }

  const AtomicString& InterfaceName() const override;
  bool IsActivateInvisibleEvent() const override;

  void Trace(Visitor*) override;

 private:
  explicit ActivateInvisibleEvent(Element* activated_element);

  Member<Element> activated_element_;
};

DEFINE_EVENT_TYPE_CASTS(ActivateInvisibleEvent);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_INVISIBLE_DOM_ACTIVATE_INVISIBLE_EVENT_H_
