// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_RESOLVER_SELECTOR_FILTER_PARENT_SCOPE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_RESOLVER_SELECTOR_FILTER_PARENT_SCOPE_H_

#include "third_party/blink/renderer/core/css/resolver/style_resolver.h"
#include "third_party/blink/renderer/core/css/selector_filter.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/element.h"

namespace blink {

// Maintains the parent element stack (and bloom filter) inside RecalcStyle.
// SelectorFilterParentScope for the parent element is added to the stack before
// recalculating style for its children. The bloom filter is populated lazily by
// PushParentIfNeeded().
class CORE_EXPORT SelectorFilterParentScope {
  STACK_ALLOCATED();

 public:
  explicit SelectorFilterParentScope(Element& parent)
      : SelectorFilterParentScope(parent, ScopeType::kParent) {}
  ~SelectorFilterParentScope();

  static void EnsureParentStackIsPushed();

 protected:
  enum class ScopeType { kParent, kAncestors };
  SelectorFilterParentScope(Element& parent, ScopeType scope);

 private:
  void PushParentIfNeeded();
  void PushAncestors(Element&);
  void PopAncestors(Element&);

  Member<Element> parent_;
  bool pushed_ = false;
  ScopeType scope_type_;
  SelectorFilterParentScope* previous_;
  Member<StyleResolver> resolver_;

  static SelectorFilterParentScope* current_scope_;
};

// When starting the style recalc from an element which is not the root, we push
// an object of this class onto the stack to push all ancestors instead of
// having to push a SelectorFilterParentScope for each of the ancestors.
class CORE_EXPORT SelectorFilterAncestorScope final
    : private SelectorFilterParentScope {
  STACK_ALLOCATED();

 public:
  explicit SelectorFilterAncestorScope(Element& parent)
      : SelectorFilterParentScope(parent, ScopeType::kAncestors) {}
};

inline SelectorFilterParentScope::SelectorFilterParentScope(
    Element& parent,
    ScopeType scope_type)
    : parent_(parent),
      scope_type_(scope_type),
      previous_(current_scope_),
      resolver_(parent.GetDocument().GetStyleResolver()) {
  DCHECK(parent.GetDocument().InStyleRecalc());
  current_scope_ = this;
}

inline SelectorFilterParentScope::~SelectorFilterParentScope() {
  current_scope_ = previous_;
  if (!pushed_)
    return;
  resolver_->GetSelectorFilter().PopParent(*parent_);
  if (scope_type_ == ScopeType::kAncestors)
    PopAncestors(*parent_);
}

inline void SelectorFilterParentScope::EnsureParentStackIsPushed() {
  if (current_scope_)
    current_scope_->PushParentIfNeeded();
}

inline void SelectorFilterParentScope::PushParentIfNeeded() {
  if (pushed_)
    return;
  DCHECK(!previous_ || scope_type_ == ScopeType::kParent);
  if (previous_)
    previous_->PushParentIfNeeded();
  if (scope_type_ == ScopeType::kAncestors)
    PushAncestors(*parent_);
  resolver_->GetSelectorFilter().PushParent(*parent_);
  pushed_ = true;
}

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_RESOLVER_SELECTOR_FILTER_PARENT_SCOPE_H_
