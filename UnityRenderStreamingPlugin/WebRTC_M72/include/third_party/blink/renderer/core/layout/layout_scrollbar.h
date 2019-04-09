/*
 * Copyright (C) 2008, 2009 Apple Inc. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_LAYOUT_SCROLLBAR_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_LAYOUT_SCROLLBAR_H_

#include "third_party/blink/renderer/core/scroll/scrollbar.h"
#include "third_party/blink/renderer/core/style/computed_style_constants.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/hash_map.h"

namespace blink {

class ComputedStyle;
class Element;
class LayoutBox;
class LayoutObject;
class LayoutScrollbarPart;

class LayoutScrollbar final : public Scrollbar {
 public:
  static Scrollbar* CreateCustomScrollbar(ScrollableArea*,
                                          ScrollbarOrientation,
                                          Element*);

  LayoutScrollbar(ScrollableArea*, ScrollbarOrientation, Element*);
  ~LayoutScrollbar() override;

  // Return the thickness that a custom scrollbar would have, without actually
  // constructing the scrollbar.
  static int HypotheticalScrollbarThickness(ScrollbarOrientation,
                                            const LayoutBox& enclosing_box,
                                            const LayoutObject& style_source);

  // The Element that supplies our style information. If the scrollbar is
  // for a document, this is either the <body> or <html> element. Otherwise, it
  // is the element that owns our PaintLayerScrollableArea.
  Element* StyleSource() const { return style_source_.Get(); }

  IntRect ButtonRect(ScrollbarPart) const;
  IntRect TrackRect(int start_length, int end_length) const;
  IntRect TrackPieceRectWithMargins(ScrollbarPart, const IntRect&) const;

  int MinimumThumbLength() const;

  bool IsOverlayScrollbar() const override { return false; }

  LayoutScrollbarPart* GetPart(ScrollbarPart part_type) {
    return parts_.at(part_type);
  }
  const LayoutScrollbarPart* GetPart(ScrollbarPart part_type) const {
    return parts_.at(part_type);
  }

  void InvalidateDisplayItemClientsOfScrollbarParts();

  void SetVisualRect(const LayoutRect&) final;

  void Trace(blink::Visitor*) override;

 private:
  friend class Scrollbar;

  void SetEnabled(bool) override;
  void DisconnectFromScrollableArea() override;

  void SetHoveredPart(ScrollbarPart) override;
  void SetPressedPart(ScrollbarPart) override;

  void StyleChanged() override;

  bool IsCustomScrollbar() const override { return true; }

  void UpdateScrollbarParts(bool destroy = false);

  scoped_refptr<ComputedStyle> GetScrollbarPseudoStyle(ScrollbarPart, PseudoId);
  void UpdateScrollbarPart(ScrollbarPart, bool destroy = false);

  // The element that supplies our style information.
  Member<Element> style_source_;

  HashMap<unsigned, LayoutScrollbarPart*> parts_;
};

DEFINE_TYPE_CASTS(LayoutScrollbar,
                  Scrollbar,
                  scrollbar,
                  scrollbar->IsCustomScrollbar(),
                  scrollbar.IsCustomScrollbar());

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_LAYOUT_SCROLLBAR_H_
