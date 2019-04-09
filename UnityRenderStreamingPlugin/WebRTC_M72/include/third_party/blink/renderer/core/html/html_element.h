/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2004-2007, 2009, 2014 Apple Inc. All rights reserved.
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
 *
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_ELEMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_ELEMENT_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/element.h"
#include "third_party/blink/renderer/platform/text/text_direction.h"

namespace blink {

struct AttributeTriggers;
class Color;
class DocumentFragment;
class ElementInternals;
class ExceptionState;
class FormAssociated;
class HTMLFormElement;
class KeyboardEvent;
class StringOrTrustedScript;
class StringTreatNullAsEmptyStringOrTrustedScript;

enum TranslateAttributeMode {
  kTranslateAttributeYes,
  kTranslateAttributeNo,
  kTranslateAttributeInherit
};

class CORE_EXPORT HTMLElement : public Element {
  DEFINE_WRAPPERTYPEINFO();

 public:
  DECLARE_ELEMENT_FACTORY_WITH_TAGNAME(HTMLElement);

  bool HasTagName(const HTMLQualifiedName& name) const {
    return HasLocalName(name.LocalName());
  }

  String title() const final;
  int tabIndex() const override;

  void setInnerText(const String&, ExceptionState&);
  virtual void setInnerText(const StringOrTrustedScript&, ExceptionState&);
  virtual void setInnerText(const StringTreatNullAsEmptyStringOrTrustedScript&,
                            ExceptionState&);
  String innerText();
  void innerText(StringOrTrustedScript& result);
  void innerText(StringTreatNullAsEmptyStringOrTrustedScript& result);
  void setOuterText(const String&, ExceptionState&);

  virtual bool HasCustomFocusLogic() const;

  String contentEditable() const;
  void setContentEditable(const String&, ExceptionState&);
  // For HTMLElement.prototype.isContentEditable. This matches to neither
  // blink::isContentEditable() nor blink::isContentRichlyEditable().  Do not
  // use this function in Blink.
  bool isContentEditableForBinding() const;

  virtual const AtomicString& autocapitalize() const;
  void setAutocapitalize(const AtomicString&);

  virtual bool draggable() const;
  void setDraggable(bool);

  bool spellcheck() const;
  void setSpellcheck(bool);

  bool translate() const;
  void setTranslate(bool);

  const AtomicString& dir();
  void setDir(const AtomicString&);

  void click();

  void AccessKeyAction(bool send_mouse_events) override;

  bool ShouldSerializeEndTag() const;

  virtual HTMLFormElement* formOwner() const { return nullptr; }

  HTMLFormElement* FindFormAncestor() const;

  bool HasDirectionAuto() const;
  TextDirection DirectionalityIfhasDirAutoAttribute(bool& is_auto) const;

  virtual bool IsHTMLUnknownElement() const { return false; }
  virtual bool IsPluginElement() const { return false; }

  virtual bool IsLabelable() const { return false; }
  // http://www.whatwg.org/specs/web-apps/current-work/multipage/elements.html#interactive-content
  virtual bool IsInteractiveContent() const;
  void DefaultEventHandler(Event&) override;

  static const AtomicString& EventNameForAttributeName(
      const QualifiedName& attr_name);

  bool MatchesReadOnlyPseudoClass() const override;
  bool MatchesReadWritePseudoClass() const override;

  static const AtomicString& EventParameterName();

  virtual String AltText() const { return String(); }

  int offsetLeftForBinding();
  int offsetTopForBinding();
  int offsetWidthForBinding();
  int offsetHeightForBinding();

  Element* unclosedOffsetParent();

  ElementInternals* attachInternals(ExceptionState& exception_state);
  virtual FormAssociated* ToFormAssociatedOrNull() { return nullptr; }
  bool IsFormAssociatedCustomElement() const;

 protected:
  HTMLElement(const QualifiedName& tag_name, Document&, ConstructionType);

  enum AllowPercentage { kDontAllowPercentageValues, kAllowPercentageValues };
  void AddHTMLLengthToStyle(MutableCSSPropertyValueSet*,
                            CSSPropertyID,
                            const String& value,
                            AllowPercentage = kAllowPercentageValues);
  void AddHTMLColorToStyle(MutableCSSPropertyValueSet*,
                           CSSPropertyID,
                           const String& color);

  void ApplyAlignmentAttributeToStyle(const AtomicString&,
                                      MutableCSSPropertyValueSet*);
  void ApplyBorderAttributeToStyle(const AtomicString&,
                                   MutableCSSPropertyValueSet*);

  void AttributeChanged(const AttributeModificationParams&) override;
  void ParseAttribute(const AttributeModificationParams&) override;
  static bool ParseColorWithLegacyRules(const String& attribute_value,
                                        Color& parsed_color);
  bool IsPresentationAttribute(const QualifiedName&) const override;
  void CollectStyleForPresentationAttribute(
      const QualifiedName&,
      const AtomicString&,
      MutableCSSPropertyValueSet*) override;
  unsigned ParseBorderWidthAttribute(const AtomicString&) const;

  void ChildrenChanged(const ChildrenChange&) override;
  void CalculateAndAdjustDirectionality();

  InsertionNotificationRequest InsertedInto(ContainerNode&) override;
  void RemovedFrom(ContainerNode& insertion_point) override;
  void DidMoveToNewDocument(Document& old_document) override;

 private:
  String DebugNodeName() const final;
  String nodeName() const final;

  bool IsHTMLElement() const =
      delete;  // This will catch anyone doing an unnecessary check.
  bool IsStyledElement() const =
      delete;  // This will catch anyone doing an unnecessary check.

  void MapLanguageAttributeToLocale(const AtomicString&,
                                    MutableCSSPropertyValueSet*);

  DocumentFragment* TextToFragment(const String&, ExceptionState&);

  bool SelfOrAncestorHasDirAutoAttribute() const;
  void AdjustDirectionalityIfNeededAfterChildAttributeChanged(Element* child);
  void AdjustDirectionalityIfNeededAfterChildrenChanged(const ChildrenChange&);
  TextDirection Directionality(
      Node** strong_directionality_text_node = nullptr) const;

  TranslateAttributeMode GetTranslateAttributeMode() const;

  void HandleKeypressEvent(KeyboardEvent&);

  static AttributeTriggers* TriggersForAttributeName(
      const QualifiedName& attr_name);

  void OnDirAttrChanged(const AttributeModificationParams&);
  void OnInertAttrChanged(const AttributeModificationParams&);
  void OnLangAttrChanged(const AttributeModificationParams&);
  void OnNonceAttrChanged(const AttributeModificationParams&);
  void OnTabIndexAttrChanged(const AttributeModificationParams&);
  void OnXMLLangAttrChanged(const AttributeModificationParams&);
};

DEFINE_ELEMENT_TYPE_CASTS(HTMLElement, IsHTMLElement());

template <typename T>
bool IsElementOfType(const HTMLElement&);
template <>
inline bool IsElementOfType<const HTMLElement>(const HTMLElement&) {
  return true;
}

inline HTMLElement::HTMLElement(const QualifiedName& tag_name,
                                Document& document,
                                ConstructionType type = kCreateHTMLElement)
    : Element(tag_name, &document, type) {
  DCHECK(!tag_name.LocalName().IsNull());
}

inline bool Node::HasTagName(const HTMLQualifiedName& name) const {
  return IsHTMLElement() && ToHTMLElement(*this).HasTagName(name);
}

// Functor used to match HTMLElements with a specific HTML tag when using the
// ElementTraversal API.
class HasHTMLTagName {
  STACK_ALLOCATED();

 public:
  explicit HasHTMLTagName(const HTMLQualifiedName& tag_name)
      : tag_name_(tag_name) {}
  bool operator()(const HTMLElement& element) const {
    return element.HasTagName(tag_name_);
  }

 private:
  const HTMLQualifiedName& tag_name_;
};

// This requires isHTML*Element(const Element&) and isHTML*Element(const
// HTMLElement&).  When the input element is an HTMLElement, we don't need to
// check the namespace URI, just the local name.
#define DEFINE_HTMLELEMENT_TYPE_CASTS_WITH_FUNCTION(thisType)                \
  inline bool Is##thisType(const thisType* element);                         \
  inline bool Is##thisType(const thisType& element);                         \
  inline bool Is##thisType(const HTMLElement* element) {                     \
    return element && Is##thisType(*element);                                \
  }                                                                          \
  inline bool Is##thisType(const Node& node) {                               \
    return node.IsHTMLElement() ? Is##thisType(ToHTMLElement(node)) : false; \
  }                                                                          \
  inline bool Is##thisType(const Node* node) {                               \
    return node && Is##thisType(*node);                                      \
  }                                                                          \
  inline bool Is##thisType(const Element* element) {                         \
    return element && Is##thisType(*element);                                \
  }                                                                          \
  template <>                                                                \
  inline bool IsElementOfType<const thisType>(const HTMLElement& element) {  \
    return Is##thisType(element);                                            \
  }                                                                          \
  DEFINE_ELEMENT_TYPE_CASTS_WITH_FUNCTION(thisType)

}  // namespace blink

#include "third_party/blink/renderer/core/html_element_type_helpers.h"

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_ELEMENT_H_
