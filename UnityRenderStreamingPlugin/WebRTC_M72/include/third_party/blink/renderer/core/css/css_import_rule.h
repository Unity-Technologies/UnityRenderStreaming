/*
 * (C) 1999-2003 Lars Knoll (knoll@kde.org)
 * (C) 2002-2003 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2002, 2006, 2008, 2012 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_IMPORT_RULE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_IMPORT_RULE_H_

#include "third_party/blink/renderer/core/css/css_rule.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class MediaList;
class StyleRuleImport;

class CSSImportRule final : public CSSRule {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static CSSImportRule* Create(StyleRuleImport* rule, CSSStyleSheet* sheet) {
    return MakeGarbageCollected<CSSImportRule>(rule, sheet);
  }

  CSSImportRule(StyleRuleImport*, CSSStyleSheet*);
  ~CSSImportRule() override;

  String cssText() const override;
  void Reattach(StyleRuleBase*) override;

  String href() const;
  MediaList* media() const;
  CSSStyleSheet* styleSheet() const;

  void Trace(blink::Visitor*) override;

 private:
  CSSRule::Type type() const override { return kImportRule; }

  Member<StyleRuleImport> import_rule_;
  mutable Member<MediaList> media_cssom_wrapper_;
  mutable Member<CSSStyleSheet> style_sheet_cssom_wrapper_;
};

DEFINE_CSS_RULE_TYPE_CASTS(CSSImportRule, kImportRule);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_IMPORT_RULE_H_
