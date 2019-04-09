/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All
 * rights reserved.
 * Copyright (C) 2008, 2009 Torch Mobile Inc. All rights reserved.
 * (http://www.torchmobile.com/)
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_DOM_LAYOUT_TREE_BUILDER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_DOM_LAYOUT_TREE_BUILDER_H_

#include "base/memory/scoped_refptr.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/layout_tree_builder_traversal.h"
#include "third_party/blink/renderer/core/dom/node.h"
#include "third_party/blink/renderer/core/dom/text.h"
#include "third_party/blink/renderer/core/layout/layout_object.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class ComputedStyle;

// The LayoutTreeBuilder class uses the DOM tree and CSS style rules as input to
// form a LayoutObject Tree which is then used for layout computations in a
// later stage.

// To construct the LayoutObject tree, the LayoutTreeBuilder does the following:
// 1. Starting at the root of the DOM tree, traverse each visible node.
//    Visibility is determined by
//    LayoutTreeBuilderFor{Element,Text}::ShouldCreateLayoutObject() functions.
// 2. For each visible node, ensure that the style has been resolved (either by
//    getting the ComputedStyle passed on to the LayoutTreeBuilder or by forcing
//    style resolution). This is done in LayoutTreeBuilderForElement::Style().
// 3. Emit visible LayoutObjects with content and their computed styles.
//    This is dealt with by the
//    LayoutTreeBuilderFor{Element,Text}::CreateLayoutObject() functions.
template <typename NodeType>
class LayoutTreeBuilder {
  STACK_ALLOCATED();

 protected:
  LayoutTreeBuilder(NodeType& node, LayoutObject* layout_object_parent)
      : node_(node), layout_object_parent_(layout_object_parent) {
    DCHECK(!node.GetLayoutObject());
    DCHECK(node.NeedsAttach());
    DCHECK(node.GetDocument().InStyleRecalc());
    DCHECK(node.InActiveDocument());
  }

  LayoutObject* NextLayoutObject() const {
    DCHECK(layout_object_parent_);

    // Avoid an O(N^2) walk over the children when reattaching all children of a
    // node.
    if (layout_object_parent_->GetNode() &&
        layout_object_parent_->GetNode()->NeedsAttach())
      return nullptr;

    LayoutObject* next =
        LayoutTreeBuilderTraversal::NextSiblingLayoutObject(*node_);

    // If a text node is wrapped in an anonymous inline for display:contents
    // (see CreateInlineWrapperForDisplayContents()), use the wrapper as the
    // next layout object. Otherwise we would need to add code to various
    // AddChild() implementations to walk up the tree to find the correct
    // layout tree parent/siblings.
    if (next && next->IsText() && next->Parent()->IsAnonymous() &&
        next->Parent()->IsInline()) {
      return next->Parent();
    }
    return next;
  }

  Member<NodeType> node_;
  LayoutObject* layout_object_parent_;
};

class LayoutTreeBuilderForElement : public LayoutTreeBuilder<Element> {
 public:
  LayoutTreeBuilderForElement(Element&, ComputedStyle*);

  void CreateLayoutObjectIfNeeded() {
    if (ShouldCreateLayoutObject())
      CreateLayoutObject();
  }

 private:
  LayoutObject* ParentLayoutObject() const;
  LayoutObject* NextLayoutObject() const;
  bool ShouldCreateLayoutObject() const;
  void CreateLayoutObject();

  scoped_refptr<ComputedStyle> style_;
};

class LayoutTreeBuilderForText : public LayoutTreeBuilder<Text> {
 public:
  LayoutTreeBuilderForText(Text& text,
                           LayoutObject* layout_parent,
                           ComputedStyle* style_from_parent)
      : LayoutTreeBuilder(text, layout_parent), style_(style_from_parent) {}

  void CreateLayoutObject();

 private:
  LayoutObject* CreateInlineWrapperForDisplayContentsIfNeeded();

  scoped_refptr<ComputedStyle> style_;
};

// Replaces LayoutNG objects to legacy layout objects when corresponding element
// returns true for |ShouldForceLegacyLayout()|.
//
// This replacement is done by three phases:
//  1. Collects layout objects establish block formatting context(BFC) and
//     contain at least one layout object should be legacy layout.
//  2. Marks descendant LayoutNG objects of collected layout objects to
//     |NeedsReattachLayoutTree|.
//  3. Invokes |RebuildLayoutTree()| to replaces to legacy layout objects.
//
// See https://goo.gl/8s5h96 for more details.
class CORE_EXPORT ReattachLegacyLayoutObjectList final {
  // Note: Make GC-plugin happy, this object doesn't have |STACK_ALLOCATED()|.
  DISALLOW_NEW();

 public:
  ReattachLegacyLayoutObjectList(Document&);
  ~ReattachLegacyLayoutObjectList();

  void AddForceLegacyAtBFCAncestor(const LayoutObject&);
  bool IsCollecting() const;
  bool IsForcingLegacyLayout() const {
    return state_ == State::kForcingLegacyLayout;
  }
  void ForceLegacyLayoutIfNeeded();

  void Trace(blink::Visitor*);

 private:
  Member<Document> document_;

  // A list of block formatting context or layout object associated to
  // document element.
  Vector<const LayoutObject*> blocks_;

  enum class State {
    kInvalid,
    // Building legacy layout tree
    kBuildingLegacyLayoutTree,
    // Usage of this list is finished.
    kClosed,
    // Collecting block formatting context should be legacy layout.
    kCollecting,
    // Replaces LayoutNG objects to legacy layout objects.
    kForcingLegacyLayout,
  } state_ = State::kInvalid;

  DISALLOW_COPY_AND_ASSIGN(ReattachLegacyLayoutObjectList);
};

}  // namespace blink

#endif
