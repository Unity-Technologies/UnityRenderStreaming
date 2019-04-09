// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_SHAPING_SHAPE_RESULT_VIEW_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_SHAPING_SHAPE_RESULT_VIEW_H_

#include <memory>
#include "third_party/blink/renderer/platform/fonts/shaping/shape_result.h"
#include "third_party/blink/renderer/platform/fonts/simple_font_data.h"
#include "third_party/blink/renderer/platform/geometry/layout_unit.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/text/text_direction.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/noncopyable.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class ShapeResult;

// Class representing a read-only composite of views into one or more existing
// shape results.
// Implemented as a list of ref counted RunInfo instances and a start/end
// offset for each, represented using the internal RunInfoPart struct.
// This allows lines to be reference sections of the overall paragraph shape
// results without the memory or computational overhead of a copy.
//
// The example below shows the shape result and the individual lines as
// ShapeResultView instances pointing to the original paragraph results for
// the string "Pack my box with five dozen liquor jugs.":
//  ╔═════════════════════════════════════════════════════╗
//  ║ Paragraph with single run, no re-shaping for lines. ║
//  ╟─────────────────────────────────────────────────────╢
//  ║ runs_ ╭───────────────────────────────────────────╮ ║
//  ║   1:  │ Pack my box with five dozen liquor jugs.  │ ║
//  ║       ╰───────────────────────────────────────────╯ ║
//  ║ lines ╭───────────────────────────────────────────╮ ║
//  ║   1:  │ Pack my box with    -> view, run 1:  0-16 │ ║
//  ║   2:  │ five dozen liquor   -> view, run 1: 17-34 │ ║
//  ║   3:  │ jugs.               -> view, run 1: 35-40 │ ║
//  ║       ╰───────────────────────────────────────────╯ ║
//  ╚═════════════════════════════════════════════════════╝
//
// In cases where a portion of the line needs re-shaping the new results are
// added as separate runs at the beginning and/or end of the runs_ vector with a
// reference to zero or more sub-runs in the middle representing the original
// content that could be reused.
//
// In the example below the end of the first line "Jack!" needs to be re-shaped:
//  ╔═════════════════════════════════════════════════════╗
//  ║ Paragraph with single run, requiring re-shape.      ║
//  ╟─────────────────────────────────────────────────────╢
//  ║ runs_ ╭───────────────────────────────────────────╮ ║
//  ║   1:  │ "Now fax quiz Jack!" my brave ghost pled. │ ║
//  ║       ╰───────────────────────────────────────────╯ ║
//  ║ lines ╭───────────────────────────────────────────╮ ║
//  ║   1:  │ "Now fax quiz     -> view, run 1:  0-14   │ ║
//  ║   1:  │ Jack!             -> new result/run       │ ║
//  ║   2:  │ my brave ghost    -> view, run 1: 21-35   │ ║
//  ║   3:  │ pled.             -> view, run 1: 41-36   │ ║
//  ║       ╰───────────────────────────────────────────╯ ║
//  ╚═════════════════════════════════════════════════════╝
//
// In this case the beginning of the first line would be represented as a part
// referecing the a range into the original ShapeResult while the last word wold
// be a separate result owned by the ShapeResultView instance. The second
// and third lines would again be represented as parts.
class PLATFORM_EXPORT ShapeResultView final
    : public RefCounted<ShapeResultView> {
 public:
  // Create a new ShapeResultView from a pre-defined list of segments.
  // The segments list is assumed to be in logical order.
  struct Segment {
    const ShapeResult* result;
    unsigned start_index;
    unsigned end_index;
  };
  static scoped_refptr<ShapeResultView> Create(const Segment*, size_t);

  // Creates a new ShapeResultView from a single segment.
  static scoped_refptr<ShapeResultView> Create(const ShapeResult*);
  static scoped_refptr<ShapeResultView> Create(const ShapeResult*,
                                               unsigned start_index,
                                               unsigned end_index);

  ~ShapeResultView();

  scoped_refptr<ShapeResult> CreateShapeResult() const;

  unsigned StartIndex() const { return start_index_ + char_index_offset_; }
  unsigned EndIndex() const { return StartIndex() + num_characters_; }
  unsigned NumCharacters() const { return num_characters_; }
  unsigned NumGlyphs() const { return num_glyphs_; }
  float Width() const { return width_; }
  LayoutUnit SnappedWidth() const { return LayoutUnit::FromFloatCeil(width_); }
  const FloatRect& Bounds() const { return glyph_bounding_box_; }
  TextDirection Direction() const {
    return static_cast<TextDirection>(direction_);
  }
  bool Rtl() const { return Direction() == TextDirection::kRtl; }
  bool HasVerticalOffsets() const { return has_vertical_offsets_; }
  void FallbackFonts(HashSet<const SimpleFontData*>* fallback) const;

  unsigned PreviousSafeToBreakOffset(unsigned index) const;

  float ForEachGlyph(float initial_advance, GlyphCallback, void* context) const;
  float ForEachGlyph(float initial_advance,
                     unsigned from,
                     unsigned to,
                     unsigned index_offset,
                     GlyphCallback,
                     void* context) const;

  float ForEachGraphemeClusters(const StringView& text,
                                float initial_advance,
                                unsigned from,
                                unsigned to,
                                unsigned index_offset,
                                GraphemeClusterCallback,
                                void* context) const;

  scoped_refptr<const SimpleFontData> PrimaryFont() const {
    return primary_font_;
  }
  void GetRunFontData(Vector<ShapeResult::RunFontData>*) const;

 private:
  ShapeResultView(const ShapeResult*);
  unsigned ComputeStartIndex() const;

  struct RunInfoPart;
  void CreateViewsForResult(const ShapeResult*,
                            unsigned start_index,
                            unsigned end_index);
  void AddSegments(const Segment*, size_t);
  template <bool is_horizontal_run>
  void ComputeBoundsForPart(const RunInfoPart&, float origin);

  scoped_refptr<const SimpleFontData> primary_font_;

  mutable unsigned start_index_;  // Cached and updated by ComputeStartIndex.
  unsigned num_characters_;
  unsigned num_glyphs_ : 30;

  // Overall direction for the TextRun, dictates which order each individual
  // sub run (represented by RunInfo structs in the m_runs vector) can
  // have a different text direction.
  unsigned direction_ : 1;

  // Tracks whether any runs contain glyphs with a y-offset != 0.
  unsigned has_vertical_offsets_ : 1;

  // Offset of the first component added to the view. Used for compatibility
  // with ShapeResult::SubRange
  unsigned char_index_offset_;

  float width_;
  FloatRect glyph_bounding_box_;
  Vector<std::unique_ptr<RunInfoPart>, 4> parts_;

  friend class ShapeResult;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_SHAPING_SHAPE_RESULT_VIEW_H_
