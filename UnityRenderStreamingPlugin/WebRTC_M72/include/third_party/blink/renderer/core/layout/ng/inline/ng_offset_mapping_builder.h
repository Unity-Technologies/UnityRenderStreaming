// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NGOffsetMappingBuilder_h
#define NGOffsetMappingBuilder_h

#include "base/auto_reset.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/layout/ng/inline/ng_offset_mapping.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class LayoutObject;

// This is the helper class for constructing the DOM-to-TextContent offset
// mapping. It holds an offset mapping, and provides APIs to modify the mapping
// step by step until the construction is finished.
// Design doc: https://goo.gl/CJbxky
// TODO(xiaochengh): Change the mock implemetation to a real one.
class CORE_EXPORT NGOffsetMappingBuilder {
  STACK_ALLOCATED();

 public:
  // A scope-like object that, mappings appended inside the scope are marked as
  // from the given source node. When multiple scopes nest, only the inner-most
  // scope is effective. Note that at most one of the nested scopes may have a
  // non-null node.
  //
  // Example:
  //
  // NGOffsetMappingBuilder builder;
  //
  // {
  //   NGOffsetMappingBuilder::SourceNodeScope scope(&builder, node);
  //
  //   // These 3 characters are marked as from source node |node|.
  //   builder.AppendIdentity(3);
  //
  //   {
  //     NGOffsetMappingBuilder::SourceNodeScope unset_scope(&builder, nullptr);
  //
  //     // This character is marked as having no source node.
  //     builder.AppendCollapsed(1);
  //   }
  //
  //   // These 2 characters are marked as from source node |node|.
  //   builder.AppendIdentity(2);
  //
  //   // Not allowed.
  //   // NGOffsetMappingBuilder::SourceNodeScope scope(&builder, node2);
  // }
  //
  // // This character is marked as having no source node.
  // builder.AppendIdentity(1);
  class SourceNodeScope {
    STACK_ALLOCATED();

   public:
    SourceNodeScope(NGOffsetMappingBuilder* builder, const LayoutObject* node);
    ~SourceNodeScope();

   private:
    NGOffsetMappingBuilder* const builder_ = nullptr;
    base::AutoReset<Persistent<const Node>> layout_object_auto_reset_;
    base::AutoReset<unsigned> appended_length_auto_reset_;

    DISALLOW_COPY_AND_ASSIGN(SourceNodeScope);
  };

  NGOffsetMappingBuilder();

  void ReserveCapacity(unsigned capacity);

  // Append an identity offset mapping of the specified length with null
  // annotation to the builder.
  void AppendIdentityMapping(unsigned length);

  // Append a collapsed offset mapping from the specified length with null
  // annotation to the builder.
  void AppendCollapsedMapping(unsigned length);

  // TODO(xiaochengh): Add the following API when we start to fix offset mapping
  // for text-transform.
  // Append an expanded offset mapping to the specified length with null
  // annotation to the builder.
  // void AppendExpandedMapping(unsigned length);

  // This function should only be called by NGInlineItemsBuilder during
  // whitespace collapsing, and in the case that the target string of the
  // currently held mapping:
  // (i)  has at least |space_offset + 1| characters,
  // (ii) character at |space_offset| in destination string is a collapsible
  //      whitespace,
  // This function changes the space into collapsed.
  void CollapseTrailingSpace(unsigned space_offset);

  // Concatenate the offset mapping held by another builder to this builder.
  // TODO(xiaochengh): Implement when adding support for 'text-transform'
  // void Concatenate(const NGOffsetMappingBuilder&);

  // Composite the offset mapping held by another builder to this builder.
  // TODO(xiaochengh): Implement when adding support for 'text-transform'
  // void Composite(const NGOffsetMappingBuilder&);

  // Set the destination string of the offset mapping.
  void SetDestinationString(String);

  // Called when entering a non-atomic inline node (e.g., SPAN), before
  // collecting any of its inline descendants.
  void EnterInline(const LayoutObject&);

  // Called when exiting a non-atomic inline node (e.g., SPAN), after having
  // collected all of its inline descendants.
  void ExitInline(const LayoutObject&);

  // Finalize and return the offset mapping.
  // This method can only be called once, as it can invalidate the stored data.
  NGOffsetMapping Build();

 private:
  // Helper function for CollapseTrailingSpace() to maintain unit ranges.
  void ShiftRanges(unsigned position, int delta);

  Persistent<const Node> current_node_ = nullptr;
  unsigned current_offset_ = 0;
  bool has_open_unit_ = false;
#if DCHECK_IS_ON()
  bool has_nonnull_node_scope_ = false;
#endif

  // Length of the current destination string.
  unsigned destination_length_ = 0;

  // Mapping units of the current mapping function.
  Vector<NGOffsetMappingUnit> mapping_units_;

  // Unit ranges of the current mapping function.
  NGOffsetMapping::RangeMap unit_ranges_;

  // Unit range starts of currently entered inline elements.
  Vector<unsigned> open_inlines_;

  // The destination string of the offset mapping.
  String destination_string_;

  friend class SourceNodeScope;

  DISALLOW_COPY_AND_ASSIGN(NGOffsetMappingBuilder);
};

}  // namespace blink

#endif  // OffsetMappingBuilder_h
