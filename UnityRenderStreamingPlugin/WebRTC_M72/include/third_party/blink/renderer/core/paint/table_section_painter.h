// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_TABLE_SECTION_PAINTER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_TABLE_SECTION_PAINTER_H_

#include "third_party/blink/renderer/core/paint/paint_phase.h"
#include "third_party/blink/renderer/core/style/shadow_data.h"
#include "third_party/blink/renderer/platform/geometry/layout_rect.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace blink {

class CellSpan;
class LayoutPoint;
class LayoutTableCell;
class LayoutTableSection;
struct PaintInfo;

class TableSectionPainter {
  STACK_ALLOCATED();

 public:
  TableSectionPainter(const LayoutTableSection& layout_table_section)
      : layout_table_section_(layout_table_section) {}

  void Paint(const PaintInfo&);
  void PaintCollapsedBorders(const PaintInfo&);

 private:
  void PaintObject(const PaintInfo&, const LayoutPoint& paint_offset);

  void PaintBoxDecorationBackground(const PaintInfo&,
                                    const LayoutPoint&,
                                    const CellSpan& dirtied_rows,
                                    const CellSpan& dirtied_columns);
  void PaintBackgroundsBehindCell(const LayoutTableCell&, const PaintInfo&);
  void PaintCell(const LayoutTableCell&, const PaintInfo&);

  void PaintSection(const PaintInfo&);
  void PaintCollapsedSectionBorders(const PaintInfo&);

  LayoutRect TableAlignedRect(const PaintInfo& paint_info,
                              const LayoutPoint& paint_offset);

  const LayoutTableSection& layout_table_section_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_TABLE_SECTION_PAINTER_H_
