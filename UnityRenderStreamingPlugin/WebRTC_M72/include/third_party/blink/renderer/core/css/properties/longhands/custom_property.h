// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_PROPERTIES_LONGHANDS_CUSTOM_PROPERTY_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_PROPERTIES_LONGHANDS_CUSTOM_PROPERTY_H_

#include "third_party/blink/renderer/core/css/properties/longhands/variable.h"

#include "third_party/blink/renderer/core/css/property_registration.h"

namespace blink {

class PropertyRegistry;

// Represents a custom property (both registered and unregistered).
//
// Unlike all other CSSProperty instances, instances of this class are
// allocated dynamically on demand. (See CSSPropertyRef).
//
// TODO(andruud): Move functionality from Variable to here, and eventually
// remove Variable.
class CORE_EXPORT CustomProperty : public Variable {
  DISALLOW_NEW();

 public:
  CustomProperty() = default;
  CustomProperty(const AtomicString& name, const Document&);
  CustomProperty(const AtomicString& name, const PropertyRegistry*);

  bool IsInherited() const override;
  const AtomicString& GetPropertyNameAtomicString() const override;

  void ApplyInitial(StyleResolverState&) const override;
  void ApplyInherit(StyleResolverState&) const override;
  void ApplyValue(StyleResolverState&, const CSSValue&) const override;

  const CSSValue* ParseSingleValue(CSSParserTokenRange&,
                                   const CSSParserContext&,
                                   const CSSParserLocalContext&) const override;

  const CSSValue* CSSValueFromComputedStyleInternal(
      const ComputedStyle&,
      const SVGComputedStyle&,
      const LayoutObject*,
      Node* styled_node,
      bool allow_visited_style) const override;

  void Trace(blink::Visitor* visitor) { visitor->Trace(registration_); }

 private:
  const CSSValue* ParseUntyped(CSSParserTokenRange,
                               const CSSParserContext&,
                               const CSSParserLocalContext&) const;
  const CSSValue* ParseTyped(CSSParserTokenRange,
                             const CSSParserContext&,
                             const CSSParserLocalContext&) const;

  AtomicString name_;
  Member<const PropertyRegistration> registration_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_PROPERTIES_LONGHANDS_CUSTOM_PROPERTY_H_
