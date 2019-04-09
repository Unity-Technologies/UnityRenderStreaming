// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_API_LINE_LAYOUT_LIST_MARKER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_API_LINE_LAYOUT_LIST_MARKER_H_

#include "third_party/blink/renderer/core/layout/api/line_layout_box.h"
#include "third_party/blink/renderer/core/layout/layout_list_marker.h"

namespace blink {

class LineLayoutListMarker : public LineLayoutBox {
 public:
  explicit LineLayoutListMarker(LayoutListMarker* layout_list_marker)
      : LineLayoutBox(layout_list_marker) {}

  explicit LineLayoutListMarker(const LineLayoutItem& item)
      : LineLayoutBox(item) {
    SECURITY_DCHECK(!item || item.IsListMarker());
  }

  explicit LineLayoutListMarker(std::nullptr_t) : LineLayoutBox(nullptr) {}

  LineLayoutListMarker() = default;

  bool IsInside() const { return ToListMarker()->IsInside(); }

 private:
  LayoutListMarker* ToListMarker() {
    return ToLayoutListMarker(GetLayoutObject());
  }

  const LayoutListMarker* ToListMarker() const {
    return ToLayoutListMarker(GetLayoutObject());
  }
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_API_LINE_LAYOUT_LIST_MARKER_H_
