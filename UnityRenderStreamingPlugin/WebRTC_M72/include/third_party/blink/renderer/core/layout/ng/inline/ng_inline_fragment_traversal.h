// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NGInlineFragmentTraversal_h
#define NGInlineFragmentTraversal_h

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/layout/ng/ng_physical_fragment.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class LayoutObject;
class NGPhysicalContainerFragment;

// Utility class for traversing the physical fragment tree.
class CORE_EXPORT NGInlineFragmentTraversal {
  STATIC_ONLY(NGInlineFragmentTraversal);

 public:
  // Return list of ancestors from |target| to |container|. Offsets are relative
  // to |container|.
  static Vector<NGPhysicalFragmentWithOffset> AncestorsOf(
      const NGPhysicalContainerFragment& container,
      const NGPhysicalFragment& target);

  // Return list inclusive ancestors from |target| to |container|. Offsets are
  // relative to |container|.
  static Vector<NGPhysicalFragmentWithOffset> InclusiveAncestorsOf(
      const NGPhysicalContainerFragment& container,
      const NGPhysicalFragment& target);

  // Returns list of descendants in preorder. Offsets are relative to
  // specified fragment.
  static Vector<NGPhysicalFragmentWithOffset> DescendantsOf(
      const NGPhysicalContainerFragment&);

  // Returns list of inclusive descendants in preorder. Offsets are relative to
  // specified fragment.
  static Vector<NGPhysicalFragmentWithOffset> InclusiveDescendantsOf(
      const NGPhysicalFragment&);

  // Returns list of inline fragments produced from the specified LayoutObject.
  // The search is restricted in the subtree of |container|.
  // Note: When |target| is a LayoutInline, some/all of its own box fragments
  // may be absent from the fragment tree, in which case the nearest box/text
  // descendant fragments are returned.
  // Note 2: Most callers should use the enclosing block flow fragment of
  // |target| as |container|. The only exception is
  // LayoutInline::HitTestCulledInline().
  // TODO(xiaochengh): As |container| is redundant in most cases, split this
  // function into two variants that takes/omits |container|.
  static Vector<NGPhysicalFragmentWithOffset> SelfFragmentsOf(
      const NGPhysicalContainerFragment& container,
      const LayoutObject* target);
};

}  // namespace blink

#endif  // NGInlineFragmentTraversal_h
