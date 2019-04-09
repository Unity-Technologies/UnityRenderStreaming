/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2012 Google Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_DOM_SELECTION_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_DOM_SELECTION_H_

#include "third_party/blink/renderer/core/dom/context_lifecycle_observer.h"
#include "third_party/blink/renderer/core/editing/forward.h"
#include "third_party/blink/renderer/platform/bindings/exception_state.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

namespace blink {

class ExceptionState;
class Node;
class Range;
class SetSelectionOptions;
class TreeScope;

class CORE_EXPORT DOMSelection final : public ScriptWrappable,
                                       public ContextClient {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(DOMSelection);

 public:
  static DOMSelection* Create(const TreeScope* tree_scope) {
    return MakeGarbageCollected<DOMSelection>(tree_scope);
  }

  explicit DOMSelection(const TreeScope*);

  void ClearTreeScope();

  // Safari Selection Object API
  // These methods return the valid equivalents of internal editing positions.
  Node* baseNode() const;
  unsigned baseOffset() const;
  Node* extentNode() const;
  unsigned extentOffset() const;
  String type() const;
  void setBaseAndExtent(Node* base_node,
                        unsigned base_offset,
                        Node* extent_node,
                        unsigned extent_offset,
                        ExceptionState& = ASSERT_NO_EXCEPTION);
  void modify(const String& alter,
              const String& direction,
              const String& granularity);

  // Mozilla Selection Object API
  // In Firefox, anchor/focus are the equal to the start/end of the selection,
  // but reflect the direction in which the selection was made by the user. That
  // does not mean that they are base/extent, since the base/extent don't
  // reflect expansion.
  // These methods return the valid equivalents of internal editing positions.
  Node* anchorNode() const;
  unsigned anchorOffset() const;
  Node* focusNode() const;
  unsigned focusOffset() const;
  bool isCollapsed() const;
  unsigned rangeCount() const;
  void collapse(Node*, unsigned offset, ExceptionState&);
  void collapseToEnd(ExceptionState&);
  void collapseToStart(ExceptionState&);
  void extend(Node*, unsigned offset, ExceptionState&);
  Range* getRangeAt(unsigned, ExceptionState&) const;
  void removeRange(Range*);
  void removeAllRanges();
  void addRange(Range*);
  void deleteFromDocument();
  bool containsNode(const Node*, bool partly_contained) const;
  void selectAllChildren(Node*, ExceptionState&);

  String toString();

  // Microsoft Selection Object API
  void empty();

  void Trace(blink::Visitor*) override;

 private:
  bool IsAvailable() const;

  void UpdateFrameSelection(const SelectionInDOMTree&,
                            Range*,
                            const SetSelectionOptions&) const;
  // Convenience methods for accessors, does not check owner Frame presence.
  VisibleSelection GetVisibleSelection() const;
  bool IsBaseFirstInSelection() const;
  const Position& AnchorPosition() const;

  Node* ShadowAdjustedNode(const Position&) const;
  unsigned ShadowAdjustedOffset(const Position&) const;

  bool IsValidForPosition(Node*) const;

  void AddConsoleWarning(const String& message);
  Range* PrimaryRangeOrNull() const;
  EphemeralRange CreateRangeFromSelectionEditor() const;

  bool IsSelectionOfDocument() const;
  void CacheRangeIfSelectionOfDocument(Range*) const;
  Range* DocumentCachedRange() const;
  void ClearCachedRangeIfSelectionOfDocument();

  Member<const TreeScope> tree_scope_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_DOM_SELECTION_H_
