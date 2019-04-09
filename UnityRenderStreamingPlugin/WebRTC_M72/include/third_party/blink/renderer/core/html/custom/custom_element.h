// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_CUSTOM_CUSTOM_ELEMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_CUSTOM_CUSTOM_ELEMENT_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/create_element_flags.h"
#include "third_party/blink/renderer/core/dom/element.h"
#include "third_party/blink/renderer/platform/text/character.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/ascii_ctype.h"
#include "third_party/blink/renderer/platform/wtf/text/atomic_string.h"

namespace blink {

class Document;
class Element;
class HTMLElement;
class QualifiedName;
class CustomElementDefinition;
class CustomElementReaction;
class CustomElementRegistry;

class CORE_EXPORT CustomElement {
  STATIC_ONLY(CustomElement);

 public:
  // Retrieves the CustomElementRegistry for Element, if any. This
  // may be a different object for a given element over its lifetime
  // as it moves between documents.
  static CustomElementRegistry* Registry(const Element&);
  static CustomElementRegistry* Registry(const Document&);

  static CustomElementDefinition* DefinitionForElement(const Element*);

  static void AddEmbedderCustomElementName(const AtomicString& name);

  static bool IsValidName(const AtomicString& name) {
    return IsValidName(name, true);
  }

  static bool IsValidName(const AtomicString& name,
                          bool including_embedder_names) {
    if (including_embedder_names && EmbedderCustomElementNames().Contains(name))
      return true;

    // This quickly rejects all common built-in element names.
    if (name.find('-', 1) == kNotFound)
      return false;

    if (!IsASCIILower(name[0]))
      return false;

    if (name.Is8Bit()) {
      const LChar* characters = name.Characters8();
      for (wtf_size_t i = 1; i < name.length(); ++i) {
        if (!Character::IsPotentialCustomElementName8BitChar(characters[i]))
          return false;
      }
    } else {
      const UChar* characters = name.Characters16();
      for (wtf_size_t i = 1; i < name.length();) {
        UChar32 ch;
        U16_NEXT(characters, i, name.length(), ch);
        if (!Character::IsPotentialCustomElementNameChar(ch))
          return false;
      }
    }

    return !IsHyphenatedSpecElementName(name);
  }

  static bool ShouldCreateCustomElement(const AtomicString& local_name);
  static bool ShouldCreateCustomElement(const QualifiedName&);
  static bool ShouldCreateCustomizedBuiltinElement(
      const AtomicString& local_name);
  static bool ShouldCreateCustomizedBuiltinElement(const QualifiedName&);

  // Look up a definition, and create an autonomous custom element if
  // it's found.
  static HTMLElement* CreateCustomElement(Document&,
                                          const QualifiedName&,
                                          const CreateElementFlags);

  // Creates "uncustomized" or "undefined" state element. This should be
  // used when CustomElementDefinition is not found.
  static Element* CreateUncustomizedOrUndefinedElement(
      Document&,
      const QualifiedName&,
      const CreateElementFlags,
      const AtomicString& is_value);
  static HTMLElement* CreateFailedElement(Document&, const QualifiedName&);

  static void Enqueue(Element*, CustomElementReaction*);
  static void EnqueueConnectedCallback(Element*);
  static void EnqueueDisconnectedCallback(Element*);
  static void EnqueueAdoptedCallback(Element*,
                                     Document* old_owner,
                                     Document* new_owner);
  static void EnqueueAttributeChangedCallback(Element*,
                                              const QualifiedName&,
                                              const AtomicString& old_value,
                                              const AtomicString& new_value);

  static void TryToUpgrade(Element*, bool upgrade_invisible_elements = false);

  static void AddEmbedderCustomElementNameForTesting(const AtomicString& name,
                                                     ExceptionState&);

 private:
  // Some existing specs have element names with hyphens in them,
  // like font-face in SVG. The custom elements spec explicitly
  // disallows these as custom element names.
  // https://html.spec.whatwg.org/#valid-custom-element-name
  static bool IsHyphenatedSpecElementName(const AtomicString&);

  static Vector<AtomicString>& EmbedderCustomElementNames();

  enum CreateUUCheckLevel {
    kCheckAll,
    // QualifiedName is a valid custom element name, and is_value is null.
    kQNameIsValid,
  };
  template <CreateUUCheckLevel>
  static Element* CreateUncustomizedOrUndefinedElementTemplate(
      Document&,
      const QualifiedName&,
      const CreateElementFlags,
      const AtomicString& is_value);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_CUSTOM_CUSTOM_ELEMENT_H_
