/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010 Apple Inc. All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_DRAG_CARET_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_DRAG_CARET_H_

#include <memory>

#include "base/macros.h"
#include "third_party/blink/renderer/core/dom/synchronous_mutation_observer.h"
#include "third_party/blink/renderer/core/editing/caret_display_item_client.h"
#include "third_party/blink/renderer/core/editing/position_with_affinity.h"
#include "third_party/blink/renderer/platform/graphics/paint_invalidation_reason.h"

namespace blink {

class LayoutBlock;
struct PaintInvalidatorContext;

class DragCaret final : public GarbageCollectedFinalized<DragCaret>,
                        public SynchronousMutationObserver {
  USING_GARBAGE_COLLECTED_MIXIN(DragCaret);

 public:
  static DragCaret* Create();

  DragCaret();
  virtual ~DragCaret();

  // Paint invalidation methods delegating to CaretDisplayItemClient.
  void ClearPreviousVisualRect(const LayoutBlock&);
  void LayoutBlockWillBeDestroyed(const LayoutBlock&);
  void UpdateStyleAndLayoutIfNeeded();
  void InvalidatePaint(const LayoutBlock&, const PaintInvalidatorContext&);

  bool ShouldPaintCaret(const LayoutBlock&) const;
  void PaintDragCaret(const LocalFrame*,
                      GraphicsContext&,
                      const LayoutPoint&) const;

  bool IsContentRichlyEditable() const;

  bool HasCaret() const { return position_.IsNotNull(); }
  const PositionWithAffinity& CaretPosition() { return position_; }
  void SetCaretPosition(const PositionWithAffinity&);
  void Clear() { SetCaretPosition(PositionWithAffinity()); }

  void Trace(blink::Visitor*) override;

 private:
  // Implementations of |SynchronousMutationObserver|
  void NodeChildrenWillBeRemoved(ContainerNode&) final;
  void NodeWillBeRemoved(Node&) final;

  PositionWithAffinity position_;
  const std::unique_ptr<CaretDisplayItemClient> display_item_client_;

  DISALLOW_COPY_AND_ASSIGN(DragCaret);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_DRAG_CARET_H_
