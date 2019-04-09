// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_CUSTOM_DOCUMENT_LAYOUT_DEFINITION_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_CUSTOM_DOCUMENT_LAYOUT_DEFINITION_H_

#include "third_party/blink/renderer/core/layout/custom/css_layout_definition.h"
#include "third_party/blink/renderer/platform/bindings/trace_wrapper_member.h"

namespace blink {

// A document layout definition is a struct which describes the information
// needed by the document about the author defined layout.
// https://drafts.css-houdini.org/css-layout-api/#document-layout-definition
class DocumentLayoutDefinition final
    : public GarbageCollectedFinalized<DocumentLayoutDefinition> {
 public:
  explicit DocumentLayoutDefinition(CSSLayoutDefinition*);
  virtual ~DocumentLayoutDefinition();

  const Vector<CSSPropertyID>& NativeInvalidationProperties() const {
    return layout_definition_->NativeInvalidationProperties();
  }
  const Vector<AtomicString>& CustomInvalidationProperties() const {
    return layout_definition_->CustomInvalidationProperties();
  }
  const Vector<CSSPropertyID>& ChildNativeInvalidationProperties() const {
    return layout_definition_->ChildNativeInvalidationProperties();
  }
  const Vector<AtomicString>& ChildCustomInvalidationProperties() const {
    return layout_definition_->ChildCustomInvalidationProperties();
  }

  bool RegisterAdditionalLayoutDefinition(const CSSLayoutDefinition&);

  unsigned GetRegisteredDefinitionCount() const {
    return registered_definitions_count_;
  }

  virtual void Trace(blink::Visitor*);

 private:
  bool IsEqual(const CSSLayoutDefinition&);

  Member<CSSLayoutDefinition> layout_definition_;
  unsigned registered_definitions_count_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_CUSTOM_DOCUMENT_LAYOUT_DEFINITION_H_
