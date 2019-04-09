// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_DOM_FLAT_TREE_NODE_DATA_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_DOM_FLAT_TREE_NODE_DATA_H_

#include "base/macros.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class Node;
class HTMLSlotElement;

class FlatTreeNodeData final : public GarbageCollected<FlatTreeNodeData> {
 public:
  FlatTreeNodeData() {}
  void Clear() {
    assigned_slot_ = nullptr;
    previous_in_assigned_nodes_ = nullptr;
    next_in_assigned_nodes_ = nullptr;
  }
  void Trace(Visitor*);

 private:
  void SetAssignedSlot(HTMLSlotElement* assigned_slot) {
    assigned_slot_ = assigned_slot;
  }

  void SetPreviousInAssignedNodes(Node* previous) {
    previous_in_assigned_nodes_ = previous;
  }
  void SetNextInAssignedNodes(Node* next) { next_in_assigned_nodes_ = next; }

  HTMLSlotElement* AssignedSlot() { return assigned_slot_; }
  Node* PreviousInAssignedNodes() { return previous_in_assigned_nodes_; }
  Node* NextInAssignedNodes() { return next_in_assigned_nodes_; }

  friend class FlatTreeTraversal;
  friend class HTMLSlotElement;

  WeakMember<HTMLSlotElement> assigned_slot_;
  WeakMember<Node> previous_in_assigned_nodes_;
  WeakMember<Node> next_in_assigned_nodes_;
  DISALLOW_COPY_AND_ASSIGN(FlatTreeNodeData);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_DOM_FLAT_TREE_NODE_DATA_H_
