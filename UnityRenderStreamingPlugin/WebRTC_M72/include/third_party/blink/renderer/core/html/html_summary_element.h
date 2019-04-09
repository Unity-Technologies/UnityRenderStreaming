/*
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_SUMMARY_ELEMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_SUMMARY_ELEMENT_H_

#include "third_party/blink/renderer/core/html/html_element.h"

namespace blink {

class HTMLDetailsElement;

class HTMLSummaryElement final : public HTMLElement {
 public:
  static HTMLSummaryElement* Create(Document&);

  explicit HTMLSummaryElement(Document&);

  bool IsMainSummary() const;
  bool WillRespondToMouseClickEvents() override;

  Element* MarkerControl();

 private:
  LayoutObject* CreateLayoutObject(const ComputedStyle&) override;
  void DefaultEventHandler(Event&) override;
  bool HasActivationBehavior() const override;
  void DidAddUserAgentShadowRoot(ShadowRoot&) override;
  HTMLDetailsElement* DetailsElement() const;

  bool SupportsFocus() const override;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_SUMMARY_ELEMENT_H_
