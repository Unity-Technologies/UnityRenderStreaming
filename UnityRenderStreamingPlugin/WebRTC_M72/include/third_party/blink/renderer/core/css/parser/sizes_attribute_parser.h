// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_PARSER_SIZES_ATTRIBUTE_PARSER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_PARSER_SIZES_ATTRIBUTE_PARSER_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/css/media_values.h"
#include "third_party/blink/renderer/core/css/parser/media_query_block_watcher.h"
#include "third_party/blink/renderer/core/css/parser/media_query_parser.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class CORE_EXPORT SizesAttributeParser {
  STACK_ALLOCATED();

 public:
  SizesAttributeParser(MediaValues*, const String&);

  float length();

 private:
  bool Parse(CSSParserTokenRange);
  float EffectiveSize();
  bool CalculateLengthInPixels(CSSParserTokenRange, float& result);
  bool MediaConditionMatches(const MediaQuerySet& media_condition);
  float EffectiveSizeDefaultValue();

  scoped_refptr<MediaQuerySet> media_condition_;
  Member<MediaValues> media_values_;
  float length_;
  bool length_was_set_;
  bool is_valid_;
};

}  // namespace blink

#endif
