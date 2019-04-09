// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_TEST_HELPERS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_TEST_HELPERS_H_

#include "base/memory/scoped_refptr.h"
#include "third_party/blink/renderer/core/css/rule_set.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace blink {

class Document;
class CSSStyleSheet;

namespace css_test_helpers {

// Example usage:
//
// css_test_helpers::TestStyleSheet sheet;
// sheet.addCSSRule("body { color: red} #a { position: absolute }");
// RuleSet& ruleSet = sheet.ruleSet();
// ... examine RuleSet to find the rule and test properties on it.
class TestStyleSheet {
  STACK_ALLOCATED();

 public:
  TestStyleSheet();
  ~TestStyleSheet();

  const Document& GetDocument() { return *document_; };

  void AddCSSRules(const char* rule_text, bool is_empty_sheet = false);
  RuleSet& GetRuleSet();
  CSSRuleList* CssRules();

 private:
  Persistent<Document> document_;
  Persistent<CSSStyleSheet> style_sheet_;
};

void RegisterProperty(Document& document,
                      const String& name,
                      const String& syntax,
                      const String& initial_value,
                      bool is_inherited);

}  // namespace css_test_helpers
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_TEST_HELPERS_H_
