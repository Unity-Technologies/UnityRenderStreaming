// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NGBfcRect_h
#define NGBfcRect_h

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/layout/ng/geometry/ng_bfc_offset.h"
#include "third_party/blink/renderer/core/layout/ng/geometry/ng_logical_size.h"
#include "third_party/blink/renderer/platform/geometry/layout_unit.h"

namespace blink {

// NGBfcRect is the position and size of a rect (typically a fragment)
// relative to a block formatting context.
struct CORE_EXPORT NGBfcRect {
  NGBfcRect(const NGBfcOffset& start_offset, const NGBfcOffset& end_offset)
      : start_offset(start_offset), end_offset(end_offset) {
    DCHECK_GE(end_offset.line_offset, start_offset.line_offset);
    DCHECK_GE(end_offset.block_offset, start_offset.block_offset);
  }

  bool IsEmpty() const;

  LayoutUnit LineStartOffset() const { return start_offset.line_offset; }
  LayoutUnit LineEndOffset() const { return end_offset.line_offset; }
  LayoutUnit BlockStartOffset() const { return start_offset.block_offset; }
  LayoutUnit BlockEndOffset() const { return end_offset.block_offset; }

  NGBfcOffset LineEndBlockStartOffset() const {
    return {LineEndOffset(), BlockStartOffset()};
  }

  LayoutUnit BlockSize() const {
    if (end_offset.block_offset == LayoutUnit::Max())
      return LayoutUnit::Max();

    return end_offset.block_offset - start_offset.block_offset;
  }
  LayoutUnit InlineSize() const {
    if (end_offset.line_offset == LayoutUnit::Max())
      return LayoutUnit::Max();

    return end_offset.line_offset - start_offset.line_offset;
  }
  NGLogicalSize Size() const { return {InlineSize(), BlockSize()}; }

  bool operator==(const NGBfcRect& other) const;
  bool operator!=(const NGBfcRect& other) const { return !(*this == other); }

  NGBfcOffset start_offset;
  NGBfcOffset end_offset;
};

}  // namespace blink

#endif  // NGBfcRect_h
