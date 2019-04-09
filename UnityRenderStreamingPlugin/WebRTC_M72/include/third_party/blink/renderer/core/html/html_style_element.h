/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2010 Apple Inc. ALl rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_STYLE_ELEMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_STYLE_ELEMENT_H_

#include <memory>
#include "third_party/blink/renderer/core/css/style_element.h"
#include "third_party/blink/renderer/core/dom/increment_load_event_delay_count.h"
#include "third_party/blink/renderer/core/html/html_element.h"

namespace blink {

class CORE_EXPORT HTMLStyleElement final : public HTMLElement,
                                           private StyleElement {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(HTMLStyleElement);

 public:
  static HTMLStyleElement* Create(Document&, const CreateElementFlags);

  HTMLStyleElement(Document&, const CreateElementFlags);
  ~HTMLStyleElement() override;

  using StyleElement::sheet;

  bool disabled() const;
  void setDisabled(bool);

  void Trace(blink::Visitor*) override;

 private:
  // Always call this asynchronously because this can cause synchronous
  // Document load event and JavaScript execution.
  void DispatchPendingEvent(std::unique_ptr<IncrementLoadEventDelayCount>);

  // overload from HTMLElement
  void ParseAttribute(const AttributeModificationParams&) override;
  InsertionNotificationRequest InsertedInto(ContainerNode&) override;
  void RemovedFrom(ContainerNode&) override;
  void ChildrenChanged(const ChildrenChange&) override;

  void FinishParsingChildren() override;

  bool SheetLoaded() override {
    return StyleElement::SheetLoaded(GetDocument());
  }
  void NotifyLoadedSheetAndAllCriticalSubresources(
      LoadedSheetErrorStatus) override;
  void StartLoadingDynamicSheet() override {
    StyleElement::StartLoadingDynamicSheet(GetDocument());
  }

  const AtomicString& media() const override;
  const AtomicString& type() const override;

  bool fired_load_;
  bool loaded_sheet_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_STYLE_ELEMENT_H_
