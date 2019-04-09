/*
 * Copyright (C) 2000 Lars Knoll (knoll@kde.org)
 *           (C) 2000 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 *           (C) 2004 Allan Sandfeld Jensen (kde@carewolf.com)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights
 * reserved.
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_PAINT_PHASE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_PAINT_PHASE_H_

namespace blink {

// The painting of a layer occurs in 4 phases, Each involves a recursive
// descent into the layer's layout objects in painting order:
//  1. Background phase: backgrounds and borders of all blocks are painted.
//     Inlines are not painted at all.
//     With |PaintTouchActionRects|, hit testing is "painted" during the
//     background phase (see: paint/README.md#hit-test-painting).
//  2. Float phase: floating objects are painted above block backgrounds but
//     entirely below inline content that can overlap them.
//  3. Foreground phase: all inlines are fully painted. Atomic inline elements
//     will get all 4 phases invoked on them during this phase, as if they were
//     stacking contexts (see ObjectPainter::paintAllPhasesAtomically()).
//  4. Outline phase: outlines are painted over the foreground.

enum class PaintPhase {
  // Background phase
  //
  // Paint background of the current object and non-self-painting descendants.
  kBlockBackground = 0,
  //
  // The following two values are added besides the normal
  // kBlockBackground to distinguish backgrounds for the object itself
  // and for descendants, because the two backgrounds are often painted with
  // different scroll offsets and clips.
  //
  // Paint background of the current object only.
  kSelfBlockBackgroundOnly = 1,
  // Paint backgrounds of non-self-painting descendants only. The painter should
  // call each non-self-painting child's paint method by passing
  // paintInfo.forDescendants() which converts kDescendantBlockBackgroundsOnly
  // to kBlockBackground.
  kDescendantBlockBackgroundsOnly = 2,

  // Float phase
  kFloat = 3,

  // Foreground phase
  kForeground = 4,

  // Outline phase
  //
  // Paint outline for the current object and non-self-painting descendants.
  kOutline = 5,
  //
  // Similar to the background phase, the following two values are added for
  // painting outlines of the object itself and for descendants.
  //
  // Paint outline for the current object only.
  kSelfOutlineOnly = 6,
  // Paint outlines of non-self-painting descendants only. The painter should
  // call each non-self-painting child's paint method by passing
  // paintInfo.forDescendants() which converts kDescendantOutlinesOnly to
  // kOutline.
  kDescendantOutlinesOnly = 7,

  // The below are auxiliary phases which are used to paint special effects.
  kSelection = 8,
  kTextClip = 9,
  kMask = 10,

  kMax = kMask,
  // These values must be kept in sync with DisplayItem::Type and
  // DisplayItem::typeAsDebugString().
};

inline bool ShouldPaintSelfBlockBackground(PaintPhase phase) {
  return phase == PaintPhase::kBlockBackground ||
         phase == PaintPhase::kSelfBlockBackgroundOnly;
}

inline bool ShouldPaintSelfOutline(PaintPhase phase) {
  return phase == PaintPhase::kOutline || phase == PaintPhase::kSelfOutlineOnly;
}

inline bool ShouldPaintDescendantBlockBackgrounds(PaintPhase phase) {
  return phase == PaintPhase::kBlockBackground ||
         phase == PaintPhase::kDescendantBlockBackgroundsOnly;
}

inline bool ShouldPaintDescendantOutlines(PaintPhase phase) {
  return phase == PaintPhase::kOutline ||
         phase == PaintPhase::kDescendantOutlinesOnly;
}

// Those flags are meant as global tree operations. This means
// that they should be constant for a paint phase.
enum GlobalPaintFlag {
  kGlobalPaintNormalPhase = 0,
  // Used when painting selection as part of a drag-image. This
  // flag disables a lot of the painting code and specifically
  // triggers a PaintPhaseSelection.
  kGlobalPaintSelectionOnly = 1 << 0,
  // Used when painting a drag-image or printing in order to
  // ignore the hardware layers and paint the whole tree
  // into the topmost layer.
  kGlobalPaintFlattenCompositingLayers = 1 << 1,
  // Used when printing in order to adapt the output to the medium, for
  // instance by not painting shadows and selections on text, and add
  // URL metadata for links.
  kGlobalPaintPrinting = 1 << 2
};

typedef unsigned GlobalPaintFlags;

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_PAINT_PHASE_H_
