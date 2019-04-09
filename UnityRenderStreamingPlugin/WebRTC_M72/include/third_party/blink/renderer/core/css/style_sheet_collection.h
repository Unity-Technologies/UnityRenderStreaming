/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 *           (C) 2006 Alexey Proskuryakov (ap@webkit.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2012 Apple Inc. All
 * rights reserved.
 * Copyright (C) 2008, 2009 Torch Mobile Inc. All rights reserved.
 * (http://www.torchmobile.com/)
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_STYLE_SHEET_COLLECTION_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_STYLE_SHEET_COLLECTION_H_

#include "base/macros.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/css/active_style_sheets.h"
#include "third_party/blink/renderer/platform/bindings/name_client.h"
#include "third_party/blink/renderer/platform/bindings/trace_wrapper_member.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class StyleSheet;

class CORE_EXPORT StyleSheetCollection
    : public GarbageCollected<StyleSheetCollection>,
      public NameClient {
 public:
  friend class ActiveDocumentStyleSheetCollector;
  friend class ImportedDocumentStyleSheetCollector;

  static StyleSheetCollection* Create() {
    return MakeGarbageCollected<StyleSheetCollection>();
  }

  StyleSheetCollection();

  const ActiveStyleSheetVector& ActiveAuthorStyleSheets() const {
    return active_author_style_sheets_;
  }
  const HeapVector<TraceWrapperMember<StyleSheet>>&
  StyleSheetsForStyleSheetList() const {
    return style_sheets_for_style_sheet_list_;
  }

  void Swap(StyleSheetCollection&);
  void SwapSheetsForSheetList(HeapVector<Member<StyleSheet>>&);
  void AppendActiveStyleSheet(const ActiveStyleSheet&);
  void AppendSheetForList(StyleSheet*);
  void MarkSheetListDirty() { sheet_list_dirty_ = true; }

  virtual void Trace(blink::Visitor*);
  const char* NameInHeapSnapshot() const override {
    return "StyleSheetCollection";
  }

  void Dispose();

 protected:
  HeapVector<TraceWrapperMember<StyleSheet>> style_sheets_for_style_sheet_list_;
  ActiveStyleSheetVector active_author_style_sheets_;
  bool sheet_list_dirty_ = true;

 private:
  DISALLOW_COPY_AND_ASSIGN(StyleSheetCollection);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_STYLE_SHEET_COLLECTION_H_
