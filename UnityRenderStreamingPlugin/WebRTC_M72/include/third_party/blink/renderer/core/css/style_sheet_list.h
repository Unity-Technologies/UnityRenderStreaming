/*
 * (C) 1999-2003 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2004, 2006, 2007 Apple Inc. All rights reserved.
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
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_STYLE_SHEET_LIST_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_STYLE_SHEET_LIST_H_

#include "third_party/blink/renderer/core/css/css_style_sheet.h"
#include "third_party/blink/renderer/core/dom/tree_scope.h"
#include "third_party/blink/renderer/platform/bindings/trace_wrapper_member.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class HTMLStyleElement;
class StyleSheet;

class CORE_EXPORT StyleSheetList final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static StyleSheetList* Create();
  static StyleSheetList* Create(const HeapVector<Member<CSSStyleSheet>>&,
                                ExceptionState&);

  static StyleSheetList* Create(TreeScope* tree_scope) {
    return MakeGarbageCollected<StyleSheetList>(tree_scope);
  }

  explicit StyleSheetList(const HeapVector<Member<CSSStyleSheet>>&);
  explicit StyleSheetList(TreeScope*);
  StyleSheetList() {}

  unsigned length();
  StyleSheet* item(unsigned index);

  HTMLStyleElement* GetNamedItem(const AtomicString&) const;

  Document* GetDocument() const {
    return tree_scope_ ? &tree_scope_->GetDocument() : nullptr;
  }

  CSSStyleSheet* AnonymousNamedGetter(const AtomicString&);

  void Trace(blink::Visitor*) override;

 private:
  const HeapVector<TraceWrapperMember<StyleSheet>>& StyleSheets() const;

  Member<TreeScope> tree_scope_;
  HeapVector<Member<CSSStyleSheet>> style_sheet_vector_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_STYLE_SHEET_LIST_H_
