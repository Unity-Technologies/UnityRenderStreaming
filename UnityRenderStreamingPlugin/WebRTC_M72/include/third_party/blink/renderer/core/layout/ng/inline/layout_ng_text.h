// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_NG_INLINE_LAYOUT_NG_TEXT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_NG_INLINE_LAYOUT_NG_TEXT_H_

#include "third_party/blink/renderer/core/layout/layout_text.h"
#include "third_party/blink/renderer/core/layout/ng/inline/ng_inline_items.h"

namespace blink {

// This overrides the default LayoutText to reference LayoutNGInlineItems
// instead of InlineTextBoxes.
//
class CORE_EXPORT LayoutNGText : public LayoutText {
 public:
  LayoutNGText(Node* node, scoped_refptr<StringImpl> text)
      : LayoutText(node, text) {}

  bool IsOfType(LayoutObjectType type) const override {
    return type == kLayoutObjectNGText || LayoutText::IsOfType(type);
  }
  bool IsLayoutNGObject() const override { return true; }

 protected:
  void InsertedIntoTree() override {
    valid_ng_items_ = false;
    LayoutText::InsertedIntoTree();
  }

 private:
  const NGInlineItems* GetNGInlineItems() const final { return &inline_items_; }
  NGInlineItems* GetNGInlineItems() final { return &inline_items_; }

  NGInlineItems inline_items_;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutNGText, IsLayoutNGText());

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_NG_INLINE_LAYOUT_NG_TEXT_H_
