/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_FORMS_HTML_OPTIONS_COLLECTION_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_FORMS_HTML_OPTIONS_COLLECTION_H_

#include "third_party/blink/renderer/core/html/forms/html_option_element.h"
#include "third_party/blink/renderer/core/html/html_collection.h"

namespace blink {

class ExceptionState;
class HTMLOptionElementOrHTMLOptGroupElement;
class HTMLElementOrLong;

class HTMLOptionsCollection final : public HTMLCollection {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static HTMLOptionsCollection* Create(ContainerNode&, CollectionType);

  explicit HTMLOptionsCollection(ContainerNode&);

  HTMLOptionElement* item(unsigned offset) const {
    return ToHTMLOptionElement(HTMLCollection::item(offset));
  }

  void add(const HTMLOptionElementOrHTMLOptGroupElement&,
           const HTMLElementOrLong&,
           ExceptionState&);
  void remove(int index);

  int selectedIndex() const;
  void setSelectedIndex(int);

  void setLength(unsigned, ExceptionState&);
  bool AnonymousIndexedSetter(unsigned, HTMLOptionElement*, ExceptionState&);

  bool ElementMatches(const HTMLElement&) const;

 private:
  void SupportedPropertyNames(Vector<String>& names) override;
};

DEFINE_TYPE_CASTS(HTMLOptionsCollection,
                  LiveNodeListBase,
                  collection,
                  collection->GetType() == kSelectOptions,
                  collection.GetType() == kSelectOptions);

inline bool HTMLOptionsCollection::ElementMatches(
    const HTMLElement& element) const {
  if (!IsHTMLOptionElement(element))
    return false;
  Node* parent = element.parentNode();
  if (!parent)
    return false;
  if (parent == &RootNode())
    return true;
  return IsHTMLOptGroupElement(*parent) && parent->parentNode() == &RootNode();
}

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_FORMS_HTML_OPTIONS_COLLECTION_H_
