// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_SHAPING_SHAPE_RESULT_TEST_INFO_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_SHAPING_SHAPE_RESULT_TEST_INFO_H_

#include "third_party/blink/renderer/platform/fonts/shaping/harfbuzz_shaper.h"
#include "third_party/blink/renderer/platform/fonts/shaping/shape_result_bloberizer.h"

#include <hb.h>

namespace blink {

class PLATFORM_EXPORT ShapeResultTestInfo : public ShapeResult {
 public:
  unsigned NumberOfRunsForTesting() const;
  ShapeResult::RunInfo& RunInfoForTesting(unsigned run_index) const;
  bool RunInfoForTesting(unsigned run_index,
                         unsigned& start_index,
                         unsigned& num_glyphs,
                         hb_script_t&) const;
  bool RunInfoForTesting(unsigned run_index,
                         unsigned& start_index,
                         unsigned& num_characters,
                         unsigned& num_glyphs,
                         hb_script_t&) const;
  uint16_t GlyphForTesting(unsigned run_index, unsigned glyph_index) const;
  float AdvanceForTesting(unsigned run_index, unsigned glyph_index) const;
  SimpleFontData* FontDataForTesting(unsigned run_index) const;
  Vector<unsigned> CharacterIndexesForTesting() const;
};

class PLATFORM_EXPORT ShapeResultBloberizerTestInfo {
 public:
  static const SimpleFontData* PendingRunFontData(
      const ShapeResultBloberizer& bloberizer) {
    return bloberizer.pending_font_data_;
  }

  static CanvasRotationInVertical PendingBlobRotation(
      const ShapeResultBloberizer& bloberizer) {
    return bloberizer.pending_canvas_rotation_;
  }

  static const Vector<Glyph, 1024>& PendingRunGlyphs(
      const ShapeResultBloberizer& bloberizer) {
    return bloberizer.pending_glyphs_;
  }

  static const Vector<float, 1024>& PendingRunOffsets(
      const ShapeResultBloberizer& bloberizer) {
    return bloberizer.pending_offsets_;
  }

  static bool HasPendingRunVerticalOffsets(
      const ShapeResultBloberizer& bloberizer) {
    return bloberizer.HasPendingVerticalOffsets();
  }

  static size_t PendingBlobRunCount(const ShapeResultBloberizer& bloberizer) {
    return bloberizer.builder_run_count_;
  }

  static size_t CommittedBlobCount(const ShapeResultBloberizer& bloberizer) {
    return bloberizer.blobs_.size();
  }
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_SHAPING_SHAPE_RESULT_TEST_INFO_H_
