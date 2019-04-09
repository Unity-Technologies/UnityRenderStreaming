/*
 * Copyright (C) 2004, 2005, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2007 Rob Buis <buis@kde.org>
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_SCRIPT_ELEMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_SCRIPT_ELEMENT_H_

#include "third_party/blink/renderer/core/dom/create_element_flags.h"
#include "third_party/blink/renderer/core/script/script_element_base.h"
#include "third_party/blink/renderer/core/svg/svg_element.h"
#include "third_party/blink/renderer/core/svg/svg_uri_reference.h"
#include "third_party/blink/renderer/core/svg_names.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class ScriptLoader;

class SVGScriptElement final : public SVGElement,
                               public SVGURIReference,
                               public ScriptElementBase {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(SVGScriptElement);

 public:
  static SVGScriptElement* Create(Document&, const CreateElementFlags);

  SVGScriptElement(Document&, const CreateElementFlags);

  ScriptLoader* Loader() const final { return loader_.Get(); }

#if DCHECK_IS_ON()
  bool IsAnimatableAttribute(const QualifiedName&) const override;
#endif

  bool IsScriptElement() const override { return true; }

  void Trace(blink::Visitor*) override;

 private:
  void ParseAttribute(const AttributeModificationParams&) override;
  InsertionNotificationRequest InsertedInto(ContainerNode&) override;
  void DidNotifySubtreeInsertionsToDocument() override;
  void ChildrenChanged(const ChildrenChange&) override;
  void DidMoveToNewDocument(Document& old_document) override;

  void SvgAttributeChanged(const QualifiedName&) override;
  bool IsURLAttribute(const Attribute&) const override;
  bool IsStructurallyExternal() const override { return HasSourceAttribute(); }
  void FinishParsingChildren() override;

  bool HaveLoadedRequiredResources() override;

  // ScriptElementBase overrides:
  bool AsyncAttributeValue() const override { return false; }
  String CharsetAttributeValue() const override { return String(); }
  String CrossOriginAttributeValue() const override { return String(); }
  bool DeferAttributeValue() const override { return false; }
  String EventAttributeValue() const override { return String(); }
  String ForAttributeValue() const override { return String(); }
  String IntegrityAttributeValue() const override { return String(); }
  String ReferrerPolicyAttributeValue() const override { return String(); }
  String LanguageAttributeValue() const override { return String(); }
  bool NomoduleAttributeValue() const override { return false; }
  String SourceAttributeValue() const override;
  String TypeAttributeValue() const override;
  String TextFromChildren() override;
  bool HasSourceAttribute() const override;
  bool IsConnected() const override;
  bool HasChildren() const override;
  const AtomicString& GetNonceForElement() const override;
  bool ElementHasDuplicateAttributes() const override {
    return HasDuplicateAttribute();
  }
  bool AllowInlineScriptForCSP(const AtomicString& nonce,
                               const WTF::OrdinalNumber&,
                               const String& script_content,
                               ContentSecurityPolicy::InlineType) override;
  Document& GetDocument() const override;
  void DispatchLoadEvent() override;
  void DispatchErrorEvent() override;
  void SetScriptElementForBinding(
      HTMLScriptElementOrSVGScriptElement&) override;

  Element* CloneWithoutAttributesAndChildren(Document&) const override;
  bool LayoutObjectIsNeeded(const ComputedStyle&) const override {
    return false;
  }

  bool have_fired_load_ = false;

  TraceWrapperMember<ScriptLoader> loader_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_SCRIPT_ELEMENT_H_
