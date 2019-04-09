/*
 * Copyright (C) 2004 Apple Computer, Inc.  All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_SERIALIZERS_SERIALIZATION_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_SERIALIZERS_SERIALIZATION_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/css/css_property_names.h"
#include "third_party/blink/renderer/core/dom/parser_content_policy.h"
#include "third_party/blink/renderer/core/editing/forward.h"
#include "third_party/blink/renderer/core/editing/serializers/html_interchange.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

namespace blink {

class ContainerNode;
class Document;
class DocumentFragment;
class Element;
class ExceptionState;
class Node;
class CSSPropertyValueSet;

enum EChildrenOnly { kIncludeNode, kChildrenOnly };
enum EAbsoluteURLs { kDoNotResolveURLs, kResolveAllURLs, kResolveNonLocalURLs };
enum class ConvertBlocksToInlines { kNotConvert, kConvert };

DocumentFragment* CreateFragmentFromText(const EphemeralRange& context,
                                         const String& text);
DocumentFragment* CreateFragmentFromMarkup(
    Document&,
    const String& markup,
    const String& base_url,
    ParserContentPolicy = kAllowScriptingContent);
DocumentFragment* CreateFragmentFromMarkupWithContext(Document&,
                                                      const String& markup,
                                                      unsigned fragment_start,
                                                      unsigned fragment_end,
                                                      const String& base_url,
                                                      ParserContentPolicy);
DocumentFragment* CreateFragmentForInnerOuterHTML(const String&,
                                                  Element*,
                                                  ParserContentPolicy,
                                                  const char* method,
                                                  ExceptionState&);
DocumentFragment* CreateFragmentForTransformToFragment(
    const String&,
    const String& source_mime_type,
    Document& output_doc);
DocumentFragment* CreateContextualFragment(const String&,
                                           Element*,
                                           ParserContentPolicy,
                                           ExceptionState&);

bool IsPlainTextMarkup(Node*);

// These methods are used by HTMLElement & ShadowRoot to replace the
// children with respected fragment/text.
void ReplaceChildrenWithFragment(ContainerNode*,
                                 DocumentFragment*,
                                 ExceptionState&);
void ReplaceChildrenWithText(ContainerNode*, const String&, ExceptionState&);

CORE_EXPORT String CreateMarkup(const Node*,
                                EChildrenOnly = kIncludeNode,
                                EAbsoluteURLs = kDoNotResolveURLs);

CORE_EXPORT String
CreateMarkup(const Position& start,
             const Position& end,
             EAnnotateForInterchange = kDoNotAnnotateForInterchange,
             ConvertBlocksToInlines = ConvertBlocksToInlines::kNotConvert,
             EAbsoluteURLs = kDoNotResolveURLs,
             Node* constraining_ancestor = nullptr);
CORE_EXPORT String
CreateMarkup(const PositionInFlatTree& start,
             const PositionInFlatTree& end,
             EAnnotateForInterchange = kDoNotAnnotateForInterchange,
             ConvertBlocksToInlines = ConvertBlocksToInlines::kNotConvert,
             EAbsoluteURLs = kDoNotResolveURLs,
             Node* constraining_ancestor = nullptr);

void MergeWithNextTextNode(Text*, ExceptionState&);

bool PropertyMissingOrEqualToNone(CSSPropertyValueSet*, CSSPropertyID);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_SERIALIZERS_SERIALIZATION_H_
