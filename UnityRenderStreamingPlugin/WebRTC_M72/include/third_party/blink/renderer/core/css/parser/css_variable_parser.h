// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_PARSER_CSS_VARIABLE_PARSER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_PARSER_CSS_VARIABLE_PARSER_H_

#include "base/memory/scoped_refptr.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/css/parser/css_parser_token_range.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/text/atomic_string.h"

namespace blink {

class CSSCustomPropertyDeclaration;
class CSSParserContext;
class CSSVariableReferenceValue;

class CORE_EXPORT CSSVariableParser {
 public:
  static bool ContainsValidVariableReferences(CSSParserTokenRange);

  static CSSCustomPropertyDeclaration* ParseDeclarationValue(
      const AtomicString&,
      CSSParserTokenRange,
      bool is_animation_tainted,
      const CSSParserContext&);
  static CSSVariableReferenceValue* ParseRegisteredPropertyValue(
      CSSParserTokenRange,
      const CSSParserContext&,
      bool require_var_reference,
      bool is_animation_tainted);

  static bool IsValidVariableName(const CSSParserToken&);
  static bool IsValidVariableName(const String&);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_PARSER_CSS_VARIABLE_PARSER_H_
