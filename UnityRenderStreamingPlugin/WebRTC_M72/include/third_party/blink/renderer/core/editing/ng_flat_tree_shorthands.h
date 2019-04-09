// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_NG_FLAT_TREE_SHORTHANDS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_NG_FLAT_TREE_SHORTHANDS_H_

#include "third_party/blink/renderer/core/editing/forward.h"

namespace blink {

struct NGCaretPosition;
class LayoutBlockFlow;
struct LocalCaretRect;

// This file contains shorthands that converts FlatTree-variants of editing
// objects into DOM tree variants, and then pass them to LayoutNG utility
// functions that accept DOM tree variants only.

const LayoutBlockFlow* NGInlineFormattingContextOf(const PositionInFlatTree&);

NGCaretPosition ComputeNGCaretPosition(const PositionInFlatTreeWithAffinity&);

LocalCaretRect ComputeNGLocalCaretRect(const PositionInFlatTreeWithAffinity&);

bool InSameNGLineBox(const PositionInFlatTreeWithAffinity&,
                     const PositionInFlatTreeWithAffinity&);

}  // namespace blink

#endif
