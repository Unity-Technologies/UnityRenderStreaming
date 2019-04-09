/*
 * Copyright (C) 2008, 2009 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_SEGMENTED_FONT_DATA_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_SEGMENTED_FONT_DATA_H_

#include "third_party/blink/renderer/platform/fonts/font_data.h"
#include "third_party/blink/renderer/platform/fonts/font_data_for_range_set.h"
#include "third_party/blink/renderer/platform/platform_export.h"

class SimpleFontData;

namespace blink {

class PLATFORM_EXPORT SegmentedFontData : public FontData {
 public:
  static scoped_refptr<SegmentedFontData> Create() {
    return base::AdoptRef(new SegmentedFontData);
  }

  void AppendFace(scoped_refptr<FontDataForRangeSet> font_data_for_range_set) {
    faces_.push_back(std::move(font_data_for_range_set));
  }
  unsigned NumFaces() const { return faces_.size(); }
  scoped_refptr<FontDataForRangeSet> FaceAt(unsigned i) const { return faces_[i]; }
  bool ContainsCharacter(UChar32) const;

 private:
  SegmentedFontData() = default;

  const SimpleFontData* FontDataForCharacter(UChar32) const override;

  bool IsCustomFont() const override;
  bool IsLoading() const override;
  bool IsLoadingFallback() const override;
  bool IsSegmented() const override;
  bool ShouldSkipDrawing() const override;

  Vector<scoped_refptr<FontDataForRangeSet>, 1> faces_;
};

DEFINE_FONT_DATA_TYPE_CASTS(SegmentedFontData, true);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_FONTS_SEGMENTED_FONT_DATA_H_
