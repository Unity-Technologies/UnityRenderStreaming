// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_CUSTOM_CUSTOM_ELEMENT_UPGRADE_SORTER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_CUSTOM_CUSTOM_ELEMENT_UPGRADE_SORTER_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class Element;
class Node;

// Does a shadow-including tree order sort of a subset of elements.
// https://dom.spec.whatwg.org/#concept-shadow-including-tree-order
class CORE_EXPORT CustomElementUpgradeSorter {
  STACK_ALLOCATED();

 public:
  CustomElementUpgradeSorter();

  // Record an element of interest. The DOM tree must not be
  // modified between calls to add and the call to sorted.
  void Add(Element*);

  // Adds shadow-including descendents of parent to result in
  // shadow-including tree order. This operation is destroys the
  // state of this sorter; after calling sorted, you must not call
  // add or sorted again with this object.
  void Sorted(HeapVector<Member<Element>>* result, Node* parent);

 private:
  using ChildSet = HeapHashSet<Member<Node>>;
  using ParentChildMap = HeapHashMap<Member<Node>, Member<ChildSet>>;

  enum AddResult { kParentAlreadyExistsInMap, kParentAddedToMap };

  AddResult AddToParentChildMap(Node* parent, Node* child);
  void Visit(HeapVector<Member<Element>>* result,
             ChildSet&,
             const ChildSet::iterator&);

  Member<HeapHashSet<Member<Element>>> elements_;

  // This is the subset of the tree, from root node (usually
  // document) through elements and shadow roots, to candidates.
  Member<ParentChildMap> parent_child_map_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_CUSTOM_CUSTOM_ELEMENT_UPGRADE_SORTER_H_
