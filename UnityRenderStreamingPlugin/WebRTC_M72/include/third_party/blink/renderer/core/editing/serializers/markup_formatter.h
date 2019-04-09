/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights
 * reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_SERIALIZERS_MARKUP_FORMATTER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_SERIALIZERS_MARKUP_FORMATTER_H_

#include "base/macros.h"
#include "third_party/blink/renderer/core/editing/editing_strategy.h"
#include "third_party/blink/renderer/core/editing/serializers/serialization.h"
#include "third_party/blink/renderer/platform/wtf/hash_map.h"
#include "third_party/blink/renderer/platform/wtf/text/string_builder.h"

namespace blink {

class Attribute;
class DocumentType;
class Element;
class Node;

typedef HashMap<AtomicString, AtomicString> Namespaces;

enum EntityMask {
  kEntityAmp = 0x0001,
  kEntityLt = 0x0002,
  kEntityGt = 0x0004,
  kEntityQuot = 0x0008,
  kEntityNbsp = 0x0010,
  kEntityTab = 0x0020,
  kEntityLineFeed = 0x0040,
  kEntityCarriageReturn = 0x0080,

  // Non-breaking space needs to be escaped in innerHTML for compatibility
  // reasons. See http://trac.webkit.org/changeset/32879. However, we cannot do
  // this in an XML document because it does not have the entity reference
  // defined (see bug 19215).
  kEntityMaskInCDATA = 0,
  kEntityMaskInPCDATA = kEntityAmp | kEntityLt | kEntityGt,
  kEntityMaskInHTMLPCDATA = kEntityMaskInPCDATA | kEntityNbsp,
  kEntityMaskInAttributeValue =
      kEntityAmp | kEntityQuot | kEntityLt | kEntityGt | kEntityTab |
      kEntityLineFeed |
      kEntityCarriageReturn,
  kEntityMaskInHTMLAttributeValue = kEntityAmp | kEntityQuot | kEntityNbsp,
};

enum class SerializationType { kAsOwnerDocument, kForcedXML };

class MarkupFormatter final {
  STACK_ALLOCATED();

 public:
  static void AppendAttributeValue(StringBuilder&, const String&, bool);
  static void AppendCDATASection(StringBuilder&, const String&);
  static void AppendCharactersReplacingEntities(StringBuilder&,
                                                const String&,
                                                unsigned,
                                                unsigned,
                                                EntityMask);
  static void AppendComment(StringBuilder&, const String&);
  static void AppendDocumentType(StringBuilder&, const DocumentType&);
  static void AppendNamespace(StringBuilder&,
                              const AtomicString& prefix,
                              const AtomicString& namespace_uri,
                              Namespaces&);
  static void AppendProcessingInstruction(StringBuilder&,
                                          const String& target,
                                          const String& data);
  static void AppendXMLDeclaration(StringBuilder&, const Document&);

  MarkupFormatter(EAbsoluteURLs,
                  SerializationType = SerializationType::kAsOwnerDocument);
  ~MarkupFormatter();

  void AppendStartMarkup(StringBuilder&, const Node&, Namespaces*);
  void AppendEndMarkup(StringBuilder&, const Element&);

  bool SerializeAsHTMLDocument(const Node&) const;

  void AppendText(StringBuilder&, Text&);
  void AppendOpenTag(StringBuilder&, const Element&, Namespaces*);
  void AppendCloseTag(StringBuilder&, const Element&);
  void AppendAttribute(StringBuilder&,
                       const Element&,
                       const Attribute&,
                       Namespaces*);

  bool ShouldAddNamespaceElement(const Element&, Namespaces&) const;
  bool ShouldAddNamespaceAttribute(const Attribute&, const Element&) const;
  EntityMask EntityMaskForText(const Text&) const;
  bool ShouldSelfClose(const Element&) const;

 private:
  String ResolveURLIfNeeded(const Element&, const String&) const;
  void AppendQuotedURLAttributeValue(StringBuilder&,
                                     const Element&,
                                     const Attribute&);

  const EAbsoluteURLs resolve_urls_method_;
  SerializationType serialization_type_;

  DISALLOW_COPY_AND_ASSIGN(MarkupFormatter);
};

}  // namespace blink

#endif
