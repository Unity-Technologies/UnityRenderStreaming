/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_DOM_NODE_CHILD_REMOVAL_TRACKER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_DOM_NODE_CHILD_REMOVAL_TRACKER_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/node.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class NodeChildRemovalTracker {
  STACK_ALLOCATED();

 public:
  explicit NodeChildRemovalTracker(Node&);
  ~NodeChildRemovalTracker();

  static bool IsBeingRemoved(Node*);

 private:
  Node& GetNode() const { return *node_; }
  NodeChildRemovalTracker* Previous() { return previous_; }

  Member<Node> node_;
  // Using raw pointers are safe because these NodeChildRemovalTrackers are
  // guaranteed to be on a stack.
  NodeChildRemovalTracker* previous_;
  CORE_EXPORT static NodeChildRemovalTracker* last_;
};

inline NodeChildRemovalTracker::NodeChildRemovalTracker(Node& node)
    : node_(node), previous_(last_) {
  last_ = this;
}

inline NodeChildRemovalTracker::~NodeChildRemovalTracker() {
  last_ = previous_;
}

inline bool NodeChildRemovalTracker::IsBeingRemoved(Node* node) {
  for (NodeChildRemovalTracker* removal = last_; removal;
       removal = removal->Previous()) {
    if (removal->GetNode().IsShadowIncludingInclusiveAncestorOf(node))
      return true;
  }

  return false;
}

}  // namespace blink

#endif
