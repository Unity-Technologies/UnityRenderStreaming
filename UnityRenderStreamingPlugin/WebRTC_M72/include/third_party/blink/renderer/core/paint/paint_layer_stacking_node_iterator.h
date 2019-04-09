/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_PAINT_LAYER_STACKING_NODE_ITERATOR_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_PAINT_LAYER_STACKING_NODE_ITERATOR_H_

#include "base/macros.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace blink {

enum ChildrenIteration {
  kNegativeZOrderChildren = 1,
  // Normal flow children are not mandated by CSS 2.1 but are an artifact of
  // our implementation: we allocate PaintLayers for elements that
  // are not treated as stacking contexts and thus we need to walk them
  // during painting and hit-testing.
  kNormalFlowChildren = 1 << 1,
  kPositiveZOrderChildren = 1 << 2,
  kAllChildren =
      kNegativeZOrderChildren | kNormalFlowChildren | kPositiveZOrderChildren
};

class PaintLayerStackingNode;
class PaintLayer;

// This iterator walks the PaintLayerStackingNode lists in the following order:
// NegativeZOrderChildren -> NormalFlowChildren -> PositiveZOrderChildren.
class PaintLayerStackingNodeIterator {
  STACK_ALLOCATED();

 public:
  PaintLayerStackingNodeIterator(const PaintLayerStackingNode& root,
                                 unsigned which_children);

  PaintLayer* Next();

 private:
  const PaintLayerStackingNode& root_;
  unsigned remaining_children_;
  unsigned index_;
  PaintLayer* current_normal_flow_child_;
  DISALLOW_COPY_AND_ASSIGN(PaintLayerStackingNodeIterator);
};

// This iterator is similar to PaintLayerStackingNodeIterator but it walks the
// lists in reverse order (from the last item to the first one).
class PaintLayerStackingNodeReverseIterator {
  STACK_ALLOCATED();

 public:
  PaintLayerStackingNodeReverseIterator(const PaintLayerStackingNode& root,
                                        unsigned which_children)
      : root_(root), remaining_children_(which_children) {
    SetIndexToLastItem();
  }

  PaintLayer* Next();

 private:
  void SetIndexToLastItem();

  const PaintLayerStackingNode& root_;
  unsigned remaining_children_;
  int index_;
  PaintLayer* current_normal_flow_child_;
  DISALLOW_COPY_AND_ASSIGN(PaintLayerStackingNodeReverseIterator);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_PAINT_LAYER_STACKING_NODE_ITERATOR_H_
