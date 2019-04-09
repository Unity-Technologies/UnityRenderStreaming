// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NGConstraintSpaceBuilder_h
#define NGConstraintSpaceBuilder_h

#include "base/optional.h"
#include "third_party/blink/renderer/core/layout/ng/geometry/ng_bfc_offset.h"
#include "third_party/blink/renderer/core/layout/ng/geometry/ng_logical_size.h"
#include "third_party/blink/renderer/core/layout/ng/ng_constraint_space.h"
#include "third_party/blink/renderer/core/layout/ng/ng_floats_utils.h"
#include "third_party/blink/renderer/platform/text/text_direction.h"
#include "third_party/blink/renderer/platform/text/writing_mode.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace blink {

class NGExclusionSpace;

class CORE_EXPORT NGConstraintSpaceBuilder final {
  STACK_ALLOCATED();

 public:
  // The setters on this builder are in the writing mode of parent_space.
  NGConstraintSpaceBuilder(const NGConstraintSpace& parent_space,
                           WritingMode out_writing_mode,
                           bool is_new_fc)
      : NGConstraintSpaceBuilder(parent_space.GetWritingMode(),
                                 out_writing_mode,
                                 is_new_fc) {
    // Propagate the intermediate layout bit to the child constraint space.
    if (parent_space.IsIntermediateLayout())
      space_.bitfields_.flags |= NGConstraintSpace::kIntermediateLayout;
  }

  // The setters on this builder are in the writing mode of parent_writing_mode.
  //
  // forced_orthogonal_writing_mode_root is set for constraint spaces created
  // directly from a LayoutObject. In this case parent_writing_mode isn't
  // actually the parent's, it's the same as out_writing_mode.
  // When this occurs we would miss setting the kOrthogonalWritingModeRoot flag
  // unless we force it.
  NGConstraintSpaceBuilder(WritingMode parent_writing_mode,
                           WritingMode out_writing_mode,
                           bool is_new_fc,
                           bool force_orthogonal_writing_mode_root = false)
      : space_(out_writing_mode),
        is_in_parallel_flow_(
            IsParallelWritingMode(parent_writing_mode, out_writing_mode)),
        is_new_fc_(is_new_fc),
        force_orthogonal_writing_mode_root_(
            force_orthogonal_writing_mode_root) {
    if (is_new_fc_)
      space_.bitfields_.flags |= NGConstraintSpace::kNewFormattingContext;

    if (!is_in_parallel_flow_ || force_orthogonal_writing_mode_root_)
      space_.bitfields_.flags |= NGConstraintSpace::kOrthogonalWritingModeRoot;
  }

  // If inline size is indefinite, use the fallback size for available inline
  // size for orthogonal flow roots. See:
  // https://www.w3.org/TR/css-writing-modes-3/#orthogonal-auto
  void AdjustInlineSizeIfNeeded(LayoutUnit* inline_size) const {
    DCHECK(!is_in_parallel_flow_);
    if (*inline_size != NGSizeIndefinite)
      return;
    DCHECK_NE(orthogonal_fallback_inline_size_, NGSizeIndefinite);
    *inline_size = orthogonal_fallback_inline_size_;
  }

  NGConstraintSpaceBuilder& SetAvailableSize(NGLogicalSize available_size) {
#if DCHECK_IS_ON()
    is_available_size_set_ = true;
#endif
    space_.available_size_ = available_size;

    if (UNLIKELY(!is_in_parallel_flow_)) {
      space_.available_size_.Flip();
      AdjustInlineSizeIfNeeded(&space_.available_size_.inline_size);
    }

    return *this;
  }

  NGConstraintSpaceBuilder& SetPercentageResolutionSize(
      NGLogicalSize percentage_resolution_size);

  NGConstraintSpaceBuilder& SetReplacedPercentageResolutionSize(
      NGLogicalSize replaced_percentage_resolution_size);

  // Set the fallback available inline-size for an orthogonal child. The size is
  // the inline size in the writing mode of the orthogonal child.
  NGConstraintSpaceBuilder& SetOrthogonalFallbackInlineSize(LayoutUnit size) {
    orthogonal_fallback_inline_size_ = size;
    return *this;
  }

  NGConstraintSpaceBuilder& SetFragmentainerBlockSize(LayoutUnit size) {
#if DCHECK_IS_ON()
    DCHECK(!is_fragmentainer_block_size_set_);
    is_fragmentainer_block_size_set_ = true;
#endif
    if (size != NGSizeIndefinite)
      space_.EnsureRareData()->fragmentainer_block_size = size;
    return *this;
  }

  NGConstraintSpaceBuilder& SetFragmentainerSpaceAtBfcStart(LayoutUnit space) {
#if DCHECK_IS_ON()
    DCHECK(!is_fragmentainer_space_at_bfc_start_set_);
    is_fragmentainer_space_at_bfc_start_set_ = true;
#endif
    if (space != NGSizeIndefinite)
      space_.EnsureRareData()->fragmentainer_space_at_bfc_start = space;
    return *this;
  }

  NGConstraintSpaceBuilder& SetTextDirection(TextDirection direction) {
    space_.bitfields_.direction = static_cast<unsigned>(direction);
    return *this;
  }

  NGConstraintSpaceBuilder& SetIsFixedSizeInline(bool b) {
    if (LIKELY(is_in_parallel_flow_))
      SetFlag(NGConstraintSpace::kFixedSizeInline, b);
    else
      SetFlag(NGConstraintSpace::kFixedSizeBlock, b);

    return *this;
  }

  NGConstraintSpaceBuilder& SetIsFixedSizeBlock(bool b) {
    if (LIKELY(is_in_parallel_flow_))
      SetFlag(NGConstraintSpace::kFixedSizeBlock, b);
    else
      SetFlag(NGConstraintSpace::kFixedSizeInline, b);

    return *this;
  }

  NGConstraintSpaceBuilder& SetFixedSizeBlockIsDefinite(bool b) {
    if (LIKELY(is_in_parallel_flow_ || !force_orthogonal_writing_mode_root_))
      SetFlag(NGConstraintSpace::kFixedSizeBlockIsDefinite, b);

    return *this;
  }

  NGConstraintSpaceBuilder& SetIsShrinkToFit(bool b) {
    SetFlag(NGConstraintSpace::kShrinkToFit, b);
    return *this;
  }

  NGConstraintSpaceBuilder& SetIsIntermediateLayout(bool b) {
    SetFlag(NGConstraintSpace::kIntermediateLayout, b);
    return *this;
  }

  NGConstraintSpaceBuilder& SetFragmentationType(
      NGFragmentationType fragmentation_type) {
#if DCHECK_IS_ON()
    DCHECK(!is_block_direction_fragmentation_type_set_);
    is_block_direction_fragmentation_type_set_ = true;
#endif
    if (fragmentation_type != NGFragmentationType::kFragmentNone) {
      space_.EnsureRareData()->block_direction_fragmentation_type =
          fragmentation_type;
    }
    return *this;
  }

  NGConstraintSpaceBuilder& SetSeparateLeadingFragmentainerMargins(bool b) {
    SetFlag(NGConstraintSpace::kSeparateLeadingFragmentainerMargins, b);
    return *this;
  }

  NGConstraintSpaceBuilder& SetIsAnonymous(bool b) {
    SetFlag(NGConstraintSpace::kAnonymous, b);
    return *this;
  }

  NGConstraintSpaceBuilder& SetUseFirstLineStyle(bool b) {
    SetFlag(NGConstraintSpace::kUseFirstLineStyle, b);
    return *this;
  }

  NGConstraintSpaceBuilder& SetAdjoiningFloatTypes(NGFloatTypes floats) {
    if (!is_new_fc_)
      space_.bitfields_.adjoining_floats = static_cast<unsigned>(floats);

    return *this;
  }

  NGConstraintSpaceBuilder& SetMarginStrut(const NGMarginStrut& margin_strut) {
#if DCHECK_IS_ON()
    DCHECK(!is_margin_strut_set_);
    is_margin_strut_set_ = true;
#endif
    if (!is_new_fc_ && margin_strut != NGMarginStrut())
      space_.EnsureRareData()->margin_strut = margin_strut;

    return *this;
  }

  NGConstraintSpaceBuilder& SetBfcOffset(const NGBfcOffset& bfc_offset) {
    if (!is_new_fc_) {
      if (space_.HasRareData())
        space_.rare_data_->bfc_offset = bfc_offset;
      else
        space_.bfc_offset_ = bfc_offset;
    }

    return *this;
  }
  NGConstraintSpaceBuilder& SetFloatsBfcBlockOffset(
      const base::Optional<LayoutUnit>& floats_bfc_block_offset) {
#if DCHECK_IS_ON()
    DCHECK(!is_floats_bfc_block_offset_set_);
    is_floats_bfc_block_offset_set_ = true;
#endif
    if (LIKELY(!is_new_fc_ && floats_bfc_block_offset != base::nullopt)) {
      space_.EnsureRareData()->floats_bfc_block_offset =
          floats_bfc_block_offset;
    }

    return *this;
  }

  NGConstraintSpaceBuilder& SetClearanceOffset(LayoutUnit clearance_offset) {
#if DCHECK_IS_ON()
    DCHECK(!is_clearance_offset_set_);
    is_clearance_offset_set_ = true;
#endif
    if (!is_new_fc_ && clearance_offset != LayoutUnit::Min())
      space_.EnsureRareData()->clearance_offset = clearance_offset;

    return *this;
  }

  NGConstraintSpaceBuilder& SetShouldForceClearance(bool b) {
    SetFlag(NGConstraintSpace::kForceClearance, b);
    return *this;
  }

  NGConstraintSpaceBuilder& SetTableCellChildLayoutPhase(
      NGTableCellChildLayoutPhase table_cell_child_layout_phase) {
    space_.bitfields_.table_cell_child_layout_phase =
        table_cell_child_layout_phase;
    return *this;
  }

  NGConstraintSpaceBuilder& SetExclusionSpace(
      const NGExclusionSpace& exclusion_space) {
    if (!is_new_fc_)
      space_.exclusion_space_ = exclusion_space;

    return *this;
  }

  void AddBaselineRequests(const NGBaselineRequestList requests) {
    DCHECK(baseline_requests_.IsEmpty());
    baseline_requests_.AppendVector(requests);
  }
  NGConstraintSpaceBuilder& AddBaselineRequest(
      const NGBaselineRequest request) {
    baseline_requests_.push_back(request);
    return *this;
  }

  // Creates a new constraint space.
  const NGConstraintSpace ToConstraintSpace() {
#if DCHECK_IS_ON()
    DCHECK(!to_constraint_space_called_)
        << "ToConstraintSpace should only be called once.";
    to_constraint_space_called_ = true;
#endif

    DCHECK(!is_new_fc_ || !space_.bitfields_.adjoining_floats);
    DCHECK_EQ(space_.HasFlag(NGConstraintSpace::kOrthogonalWritingModeRoot),
              !is_in_parallel_flow_ || force_orthogonal_writing_mode_root_);

    DCHECK(!force_orthogonal_writing_mode_root_ || is_in_parallel_flow_)
        << "Forced and inferred orthogonal writing mode shouldn't happen "
           "simultaneously. Inferred means the constraints are in parent "
           "writing mode, forced means they are in child writing mode.";

    space_.bitfields_.baseline_requests = baseline_requests_.Serialize();
    return std::move(space_);
  }

 private:
  void SetFlag(NGConstraintSpace::ConstraintSpaceFlags mask, bool value) {
    space_.bitfields_.flags =
        (space_.bitfields_.flags & ~static_cast<unsigned>(mask)) |
        (-(int32_t)value & static_cast<unsigned>(mask));
  }

  NGConstraintSpace space_;

  // Orthogonal writing mode roots may need a fallback, to prevent available
  // inline size from being indefinite, which isn't allowed. This is the
  // available inline size in the writing mode of the orthogonal child.
  LayoutUnit orthogonal_fallback_inline_size_ = NGSizeIndefinite;

  bool is_in_parallel_flow_;
  bool is_new_fc_;
  bool force_orthogonal_writing_mode_root_;

#if DCHECK_IS_ON()
  bool is_available_size_set_ = false;
  bool is_fragmentainer_block_size_set_ = false;
  bool is_fragmentainer_space_at_bfc_start_set_ = false;
  bool is_block_direction_fragmentation_type_set_ = false;
  bool is_margin_strut_set_ = false;
  bool is_floats_bfc_block_offset_set_ = false;
  bool is_clearance_offset_set_ = false;

  bool to_constraint_space_called_ = false;
#endif

  NGBaselineRequestList baseline_requests_;
};

}  // namespace blink

#endif  // NGConstraintSpaceBuilder
