// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NGPhysicalOffset_h
#define NGPhysicalOffset_h

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/geometry/layout_point.h"
#include "third_party/blink/renderer/platform/geometry/layout_size.h"
#include "third_party/blink/renderer/platform/geometry/layout_unit.h"
#include "third_party/blink/renderer/platform/text/text_direction.h"
#include "third_party/blink/renderer/platform/text/writing_mode.h"

namespace blink {

class LayoutPoint;
class LayoutSize;
struct NGLogicalOffset;
struct NGPhysicalSize;

// NGPhysicalOffset is the position of a rect (typically a fragment) relative to
// its parent rect in the physical coordinate system.
struct CORE_EXPORT NGPhysicalOffset {
  NGPhysicalOffset() = default;
  NGPhysicalOffset(LayoutUnit left, LayoutUnit top) : left(left), top(top) {}

  LayoutUnit left;
  LayoutUnit top;

  // Converts a physical offset to a logical offset. See:
  // https://drafts.csswg.org/css-writing-modes-3/#logical-to-physical
  // @param outer_size the size of the rect (typically a fragment).
  // @param inner_size the size of the inner rect (typically a child fragment).
  NGLogicalOffset ConvertToLogical(WritingMode,
                                   TextDirection,
                                   NGPhysicalSize outer_size,
                                   NGPhysicalSize inner_size) const;

  NGPhysicalOffset operator+(const NGPhysicalOffset& other) const {
    return NGPhysicalOffset{this->left + other.left, this->top + other.top};
  }
  NGPhysicalOffset& operator+=(const NGPhysicalOffset& other) {
    *this = *this + other;
    return *this;
  }

  NGPhysicalOffset operator-(const NGPhysicalOffset& other) const {
    return NGPhysicalOffset{this->left - other.left, this->top - other.top};
  }
  NGPhysicalOffset& operator-=(const NGPhysicalOffset& other) {
    *this = *this - other;
    return *this;
  }

  bool operator==(const NGPhysicalOffset& other) const {
    return other.left == left && other.top == top;
  }

  // Conversions from/to existing code. New code prefers type safety for
  // logical/physical distinctions.
  explicit NGPhysicalOffset(const LayoutPoint& point) {
    left = point.X();
    top = point.Y();
  }
  explicit NGPhysicalOffset(const LayoutSize& size) {
    left = size.Width();
    top = size.Height();
  }

  // Conversions from/to existing code. New code prefers type safety for
  // logical/physical distinctions.
  LayoutPoint ToLayoutPoint() const { return {left, top}; }
  LayoutSize ToLayoutSize() const { return {left, top}; }

  String ToString() const;
};

CORE_EXPORT std::ostream& operator<<(std::ostream&, const NGPhysicalOffset&);

}  // namespace blink

#endif  // NGPhysicalOffset_h
