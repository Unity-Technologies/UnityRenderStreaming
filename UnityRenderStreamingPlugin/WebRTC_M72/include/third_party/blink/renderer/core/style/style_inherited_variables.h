// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_STYLE_STYLE_INHERITED_VARIABLES_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_STYLE_STYLE_INHERITED_VARIABLES_H_

#include "third_party/blink/renderer/core/css/css_value.h"
#include "third_party/blink/renderer/core/css/css_variable_data.h"
#include "third_party/blink/renderer/platform/heap/persistent.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/hash_map.h"
#include "third_party/blink/renderer/platform/wtf/ref_counted.h"
#include "third_party/blink/renderer/platform/wtf/text/atomic_string_hash.h"

namespace blink {

class CORE_EXPORT StyleInheritedVariables
    : public RefCounted<StyleInheritedVariables> {
 public:
  static scoped_refptr<StyleInheritedVariables> Create() {
    return base::AdoptRef(new StyleInheritedVariables());
  }

  scoped_refptr<StyleInheritedVariables> Copy() {
    return base::AdoptRef(new StyleInheritedVariables(*this));
  }

  bool operator==(const StyleInheritedVariables& other) const;
  bool operator!=(const StyleInheritedVariables& other) const {
    return !(*this == other);
  }

  void SetVariable(const AtomicString& name,
                   scoped_refptr<CSSVariableData> value) {
    needs_resolution_ =
        needs_resolution_ || (value && (value->NeedsVariableResolution() ||
                                        value->NeedsUrlResolution()));
    data_.Set(name, std::move(value));
  }
  CSSVariableData* GetVariable(const AtomicString& name) const;
  void RemoveVariable(const AtomicString&);

  void SetRegisteredVariable(const AtomicString&, const CSSValue*);
  const CSSValue* RegisteredVariable(const AtomicString&) const;

  // Note that not all custom property names returned here necessarily have
  // valid values, due to cycles or references to invalid variables without
  // using a fallback.
  HashSet<AtomicString> GetCustomPropertyNames() const;

  bool NeedsResolution() const { return needs_resolution_; }
  void ClearNeedsResolution() { needs_resolution_ = false; }

 private:
  StyleInheritedVariables();
  StyleInheritedVariables(StyleInheritedVariables& other);

  friend class CSSVariableResolver;

  HashMap<AtomicString, scoped_refptr<CSSVariableData>> data_;
  Persistent<HeapHashMap<AtomicString, Member<CSSValue>>> registered_data_;
  scoped_refptr<StyleInheritedVariables> root_;
  bool needs_resolution_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_STYLE_STYLE_INHERITED_VARIABLES_H_
