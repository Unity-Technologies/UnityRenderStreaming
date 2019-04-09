// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_PARSER_CSS_AT_RULE_ID_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_PARSER_CSS_AT_RULE_ID_H_

#include "third_party/blink/renderer/platform/wtf/text/string_view.h"

namespace blink {

class CSSParserContext;

enum CSSAtRuleID {
  kCSSAtRuleInvalid = 0,

  kCSSAtRuleCharset = 1,
  kCSSAtRuleFontFace = 2,
  kCSSAtRuleImport = 3,
  kCSSAtRuleKeyframes = 4,
  kCSSAtRuleMedia = 5,
  kCSSAtRuleNamespace = 6,
  kCSSAtRulePage = 7,
  kCSSAtRuleSupports = 8,
  kCSSAtRuleViewport = 9,

  kCSSAtRuleWebkitKeyframes = 10,
};

CSSAtRuleID CssAtRuleID(StringView name);

void CountAtRule(const CSSParserContext*, CSSAtRuleID);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_PARSER_CSS_AT_RULE_ID_H_
