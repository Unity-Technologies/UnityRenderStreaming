// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_NG_NG_OUTLINE_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_NG_NG_OUTLINE_UTILS_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace blink {

class ComputedStyle;
class Node;
class NGPhysicalFragment;

class CORE_EXPORT NGOutlineUtils {
  STATIC_ONLY(NGOutlineUtils);

 public:

  static bool HasPaintedOutline(const ComputedStyle& style, const Node* node);

  // First fragment paints the entire outline for LayoutInline.
  // Returns true if this is the painting fragment.
  static bool IsInlineOutlineNonpaintingFragment(
      const NGPhysicalFragment& physical_fragment);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_NG_NG_OUTLINE_UTILS_H_
