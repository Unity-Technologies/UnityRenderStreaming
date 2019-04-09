// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_NG_INLINE_NG_INLINE_ITEMS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_NG_INLINE_NG_INLINE_ITEMS_H_

#include "base/macros.h"
#include "third_party/blink/renderer/core/layout/ng/inline/ng_inline_item.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class NGInlineItem;

// A collection of |NGInlineItem| associated to |LayoutNGText|.
//
// ***** INLINE ITEMS OWNERSHIP *****
// NGInlineItems in items_ are not owned by LayoutText but are pointers into the
// LayoutNGBlockFlow's items_. Should not be accessed outside of layout.
class NGInlineItems final {
 public:
  NGInlineItems() = default;

  void Add(NGInlineItem* item) { items_.push_back(item); }
  void Clear() { items_.clear(); }
  const Vector<NGInlineItem*>& Items() const { return items_; }

 private:
  Vector<NGInlineItem*> items_;

  DISALLOW_COPY_AND_ASSIGN(NGInlineItems);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_NG_INLINE_NG_INLINE_ITEMS_H_
