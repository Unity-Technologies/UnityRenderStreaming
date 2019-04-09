// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NGConstraintSpace_h
#define NGConstraintSpace_h

#include "base/optional.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/layout/ng/exclusions/ng_exclusion_space.h"
#include "third_party/blink/renderer/core/layout/ng/geometry/ng_bfc_offset.h"
#include "third_party/blink/renderer/core/layout/ng/geometry/ng_logical_size.h"
#include "third_party/blink/renderer/core/layout/ng/geometry/ng_margin_strut.h"
#include "third_party/blink/renderer/core/layout/ng/geometry/ng_physical_size.h"
#include "third_party/blink/renderer/core/layout/ng/inline/ng_baseline.h"
#include "third_party/blink/renderer/core/layout/ng/ng_floats_utils.h"
#include "third_party/blink/renderer/platform/text/text_direction.h"
#include "third_party/blink/renderer/platform/text/writing_mode.h"
#include "third_party/blink/renderer/platform/wtf/ref_counted.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class LayoutBox;
class NGConstraintSpaceBuilder;

enum NGFragmentationType {
  kFragmentNone,
  kFragmentPage,
  kFragmentColumn,
  kFragmentRegion
};

// Tables have two passes, a "measure" phase (for determining the table row
// height), and a "layout" phase.
// See: https://drafts.csswg.org/css-tables-3/#row-layout
//
// This enum is used for communicating to *direct* children of table cells,
// which layout phase the table cell is in.
enum NGTableCellChildLayoutPhase {
  kNotTableCellChild,  // The node isn't a table cell child.
  kMeasure,  // The node is a table cell child, in the "measure" phase.
  kLayout    // The node is a table cell child, in the "layout" phase.
};

// Percentages are frequently the same as the available-size, zero, or
// indefinite (thanks non-quirks mode)! This enum encodes this information.
enum NGPercentageStorage {
  kSameAsAvailable,
  kZero,
  kIndefinite,
  kRareDataPercentage
};

// The NGConstraintSpace represents a set of constraints and available space
// which a layout algorithm may produce a NGFragment within.
class CORE_EXPORT NGConstraintSpace final {
  USING_FAST_MALLOC(NGConstraintSpace);

 public:
  enum ConstraintSpaceFlags {
    kOrthogonalWritingModeRoot = 1 << 0,
    kFixedSizeInline = 1 << 1,
    kFixedSizeBlock = 1 << 2,
    kFixedSizeBlockIsDefinite = 1 << 3,
    kShrinkToFit = 1 << 4,
    kIntermediateLayout = 1 << 5,
    kSeparateLeadingFragmentainerMargins = 1 << 6,
    kNewFormattingContext = 1 << 7,
    kAnonymous = 1 << 8,
    kUseFirstLineStyle = 1 << 9,
    kForceClearance = 1 << 10,
    kHasRareData = 1 << 11,

    // Size of bitfield used to store the flags.
    kNumberOfConstraintSpaceFlags = 12
  };

  // To ensure that the bfc_offset_, rare_data_ union doesn't get polluted,
  // always initialize the bfc_offset_.
  NGConstraintSpace() : bfc_offset_() {}

  NGConstraintSpace(const NGConstraintSpace& other)
      : available_size_(other.available_size_),
        exclusion_space_(other.exclusion_space_),
        bitfields_(other.bitfields_) {
    if (HasRareData())
      rare_data_ = new RareData(*other.rare_data_);
    else
      bfc_offset_ = other.bfc_offset_;
  }
  NGConstraintSpace(NGConstraintSpace&& other)
      : available_size_(other.available_size_),
        exclusion_space_(std::move(other.exclusion_space_)),
        bitfields_(other.bitfields_) {
    if (HasRareData()) {
      rare_data_ = other.rare_data_;
      other.rare_data_ = nullptr;
    } else {
      bfc_offset_ = other.bfc_offset_;
    }
  }

  NGConstraintSpace& operator=(const NGConstraintSpace& other) {
    available_size_ = other.available_size_;
    if (HasRareData())
      delete rare_data_;
    if (other.HasRareData())
      rare_data_ = new RareData(*other.rare_data_);
    else
      bfc_offset_ = other.bfc_offset_;
    exclusion_space_ = other.exclusion_space_;
    bitfields_ = other.bitfields_;
    return *this;
  }
  NGConstraintSpace& operator=(NGConstraintSpace&& other) {
    available_size_ = other.available_size_;
    if (HasRareData())
      delete rare_data_;
    if (other.HasRareData()) {
      rare_data_ = other.rare_data_;
      other.rare_data_ = nullptr;
    } else {
      bfc_offset_ = other.bfc_offset_;
    }
    exclusion_space_ = std::move(other.exclusion_space_);
    bitfields_ = other.bitfields_;
    return *this;
  }

  ~NGConstraintSpace() {
    if (HasRareData())
      delete rare_data_;
  }

  // Creates NGConstraintSpace representing LayoutObject's containing block.
  // This should live on NGBlockNode or another layout bridge and probably take
  // a root NGConstraintSpace.
  static NGConstraintSpace CreateFromLayoutObject(const LayoutBox&);

  const NGExclusionSpace& ExclusionSpace() const { return exclusion_space_; }

  TextDirection Direction() const {
    return static_cast<TextDirection>(bitfields_.direction);
  }

  WritingMode GetWritingMode() const {
    return static_cast<WritingMode>(bitfields_.writing_mode);
  }

  bool IsOrthogonalWritingModeRoot() const {
    return HasFlag(kOrthogonalWritingModeRoot);
  }

  // The available space size.
  // See: https://drafts.csswg.org/css-sizing/#available
  NGLogicalSize AvailableSize() const { return available_size_; }

  // The size to use for percentage resolution.
  // See: https://drafts.csswg.org/css-sizing/#percentage-sizing
  LayoutUnit PercentageResolutionInlineSize() const {
    switch (static_cast<NGPercentageStorage>(
        bitfields_.percentage_inline_storage)) {
      default:
        NOTREACHED();
        U_FALLTHROUGH;
      case kSameAsAvailable:
        return available_size_.inline_size;
      case kZero:
        return LayoutUnit();
      case kIndefinite:
        return NGSizeIndefinite;
      case kRareDataPercentage:
        DCHECK(HasRareData());
        return rare_data_->percentage_resolution_size.inline_size;
    }
  }

  LayoutUnit PercentageResolutionBlockSize() const {
    switch (
        static_cast<NGPercentageStorage>(bitfields_.percentage_block_storage)) {
      default:
        NOTREACHED();
        U_FALLTHROUGH;
      case kSameAsAvailable:
        return available_size_.block_size;
      case kZero:
        return LayoutUnit();
      case kIndefinite:
        return NGSizeIndefinite;
      case kRareDataPercentage:
        DCHECK(HasRareData());
        return rare_data_->percentage_resolution_size.block_size;
    }
  }

  NGLogicalSize PercentageResolutionSize() const {
    return {PercentageResolutionInlineSize(), PercentageResolutionBlockSize()};
  }

  LayoutUnit ReplacedPercentageResolutionInlineSize() const {
    switch (static_cast<NGPercentageStorage>(
        bitfields_.replaced_percentage_inline_storage)) {
      case kSameAsAvailable:
        return available_size_.inline_size;
      case kZero:
        return LayoutUnit();
      case kIndefinite:
        return NGSizeIndefinite;
      case kRareDataPercentage:
        DCHECK(HasRareData());
        return rare_data_->replaced_percentage_resolution_size.inline_size;
      default:
        NOTREACHED();
    }

    return available_size_.inline_size;
  }

  LayoutUnit ReplacedPercentageResolutionBlockSize() const {
    switch (static_cast<NGPercentageStorage>(
        bitfields_.replaced_percentage_block_storage)) {
      case kSameAsAvailable:
        return available_size_.block_size;
      case kZero:
        return LayoutUnit();
      case kIndefinite:
        return NGSizeIndefinite;
      case kRareDataPercentage:
        DCHECK(HasRareData());
        return rare_data_->replaced_percentage_resolution_size.block_size;
      default:
        NOTREACHED();
    }

    return available_size_.block_size;
  }

  // The size to use for percentage resolution of replaced elements.
  NGLogicalSize ReplacedPercentageResolutionSize() const {
    return {ReplacedPercentageResolutionInlineSize(),
            ReplacedPercentageResolutionBlockSize()};
  }

  // The size to use for percentage resolution for margin/border/padding.
  // They are always get computed relative to the inline size, in the parent
  // writing mode.
  LayoutUnit PercentageResolutionInlineSizeForParentWritingMode() const {
    if (!IsOrthogonalWritingModeRoot())
      return PercentageResolutionInlineSize();
    if (PercentageResolutionBlockSize() != NGSizeIndefinite)
      return PercentageResolutionBlockSize();
    // TODO(mstensho): Figure out why we get here. It seems wrong, but we do get
    // here in some grid layout situations.
    return LayoutUnit();
  }

  LayoutUnit FragmentainerBlockSize() const {
    return HasRareData() ? rare_data_->fragmentainer_block_size
                         : NGSizeIndefinite;
  }

  // Return the block space that was available in the current fragmentainer at
  // the start of the current block formatting context. Note that if the start
  // of the current block formatting context is in a previous fragmentainer, the
  // size of the current fragmentainer is returned instead.
  LayoutUnit FragmentainerSpaceAtBfcStart() const {
    DCHECK(HasBlockFragmentation());
    return HasRareData() ? rare_data_->fragmentainer_space_at_bfc_start
                         : NGSizeIndefinite;
  }

  // Whether the current constraint space is for the newly established
  // Formatting Context.
  bool IsNewFormattingContext() const { return HasFlag(kNewFormattingContext); }

  // Return true if we are to separate (i.e. honor, rather than collapse)
  // block-start margins at the beginning of fragmentainers. This only makes a
  // difference if we're block-fragmented (pagination, multicol, etc.). Then
  // block-start margins at the beginning of a fragmentainers are to be
  // truncated to 0 if they occur after a soft (unforced) break.
  bool HasSeparateLeadingFragmentainerMargins() const {
    return HasFlag(kSeparateLeadingFragmentainerMargins);
  }

  // Whether the fragment produced from layout should be anonymous, (e.g. it
  // may be a column in a multi-column layout). In such cases it shouldn't have
  // any borders or padding.
  bool IsAnonymous() const { return HasFlag(kAnonymous); }

  // Whether to use the ':first-line' style or not.
  // Note, this is not about the first line of the content to layout, but
  // whether the constraint space itself is on the first line, such as when it's
  // an inline block.
  // Also note this is true only when the document has ':first-line' rules.
  bool UseFirstLineStyle() const { return HasFlag(kUseFirstLineStyle); }

  // Some layout modes “stretch” their children to a fixed size (e.g. flex,
  // grid). These flags represented whether a layout needs to produce a
  // fragment that satisfies a fixed constraint in the inline and block
  // direction respectively.
  //
  // If these flags are true, the AvailableSize() is interpreted as the fixed
  // border-box size of this box in the respective dimension.
  bool IsFixedSizeInline() const { return HasFlag(kFixedSizeInline); }

  bool IsFixedSizeBlock() const { return HasFlag(kFixedSizeBlock); }

  // Whether a fixed block size should be considered definite.
  bool FixedSizeBlockIsDefinite() const {
    return HasFlag(kFixedSizeBlockIsDefinite);
  }

  // Whether an auto inline-size should be interpreted as shrink-to-fit
  // (ie. fit-content). This is used for inline-block, floats, etc.
  bool IsShrinkToFit() const { return HasFlag(kShrinkToFit); }

  // Whether this constraint space is used for an intermediate layout in a
  // multi-pass layout. In such a case, we should not copy back the resulting
  // layout data to the legacy tree or create a paint fragment from it.
  bool IsIntermediateLayout() const { return HasFlag(kIntermediateLayout); }

  // If specified a layout should produce a Fragment which fragments at the
  // blockSize if possible.
  NGFragmentationType BlockFragmentationType() const {
    return HasRareData() ? static_cast<NGFragmentationType>(
                               rare_data_->block_direction_fragmentation_type)
                         : kFragmentNone;
  }

  // Return true if this constraint space participates in a fragmentation
  // context.
  bool HasBlockFragmentation() const {
    return BlockFragmentationType() != kFragmentNone;
  }

  // Returns if this node is a table cell child, and which table layout phase
  // is occurring.
  NGTableCellChildLayoutPhase TableCellChildLayoutPhase() const {
    return static_cast<NGTableCellChildLayoutPhase>(
        bitfields_.table_cell_child_layout_phase);
  }

  NGMarginStrut MarginStrut() const {
    return HasRareData() ? rare_data_->margin_strut : NGMarginStrut();
  }

  // The BfcOffset is where the MarginStrut is placed within the block
  // formatting context.
  //
  // The current layout or a descendant layout may "resolve" the BFC offset,
  // i.e. decide where the current fragment should be placed within the BFC.
  //
  // This is done by:
  //   bfc_block_offset =
  //     space.BfcOffset().block_offset + space.MarginStrut().Sum();
  //
  // The BFC offset can get "resolved" in many circumstances (including, but
  // not limited to):
  //   - block_start border or padding in the current layout.
  //   - Text content, atomic inlines, (see NGLineBreaker).
  //   - The current layout having a block_size.
  //   - Clearance before a child.
  NGBfcOffset BfcOffset() const {
    return HasRareData() ? rare_data_->bfc_offset : bfc_offset_;
  }

  // If present, and the current layout hasn't resolved its BFC offset yet (see
  // BfcOffset), the layout should position all of its unpositioned floats at
  // this offset. This value is the BFC offset that we calculated in the
  // previous pass, a pass which aborted once the BFC offset got resolved,
  // because we had walked past content (i.e. floats) that depended on it being
  // resolved.
  //
  // This value should be propogated to child layouts if the current layout
  // hasn't resolved its BFC offset yet.
  //
  // This value is calculated *after* an initial pass of the tree, and should
  // only be present during subsequent passes.
  base::Optional<LayoutUnit> FloatsBfcBlockOffset() const {
    return HasRareData() ? rare_data_->floats_bfc_block_offset : base::nullopt;
  }

  // Return the types (none, left, right, both) of preceding adjoining
  // floats. These are floats that are added while the in-flow BFC offset is
  // still unknown. The floats may or may not be unpositioned (pending). That
  // depends on which layout pass we're in. They are typically positioned if
  // FloatsBfcOffset() is known. Adjoining floats should be treated differently
  // when calculating clearance on a block with adjoining block-start margin.
  // (in such cases we will know up front that the block will need clearance,
  // since, if it doesn't, the float will be pulled along with the block, and
  // the block will fail to clear).
  NGFloatTypes AdjoiningFloatTypes() const {
    return bitfields_.adjoining_floats;
  }

  // Return true if there were any earlier floats that may affect the current
  // layout.
  bool HasFloats() const { return !ExclusionSpace().IsEmpty(); }

  bool HasClearanceOffset() const {
    return HasRareData() && rare_data_->clearance_offset != LayoutUnit::Min();
  }
  LayoutUnit ClearanceOffset() const {
    return HasRareData() ? rare_data_->clearance_offset : LayoutUnit::Min();
  }

  // Return true if the fragment needs to have clearance applied to it,
  // regardless of its hypothetical position. The fragment will then go exactly
  // below the relevant floats. This happens when a cleared child gets separated
  // from floats that would otherwise be adjoining; example:
  //
  // <div id="container">
  //   <div id="float" style="float:left; width:100px; height:100px;"></div>
  //   <div id="clearee" style="clear:left; margin-top:12345px;">text</div>
  // </div>
  //
  // Clearance separates #clearee from #container, and #float is positioned at
  // the block-start content edge of #container. Without clearance, margins
  // would have been adjoining and the large margin on #clearee would have
  // pulled both #container and #float along with it. No margin, no matter how
  // large, would ever be able to pull #clearee below the float then. But we
  // have clearance, the margins are separated, and in this case we know that we
  // have clearance even before we have laid out (because of the adjoining
  // float). So it would just be wrong to check for clearance when we position
  // #clearee. Nothing can prevent clearance here. A large margin on the cleared
  // child will be canceled out with negative clearance.
  bool ShouldForceClearance() const { return HasFlag(kForceClearance); }

  const NGBaselineRequestList BaselineRequests() const {
    return NGBaselineRequestList(bitfields_.baseline_requests);
  }

  // Return true if the two constraint spaces are similar enough that it *may*
  // be possible to skip re-layout. If true is returned, the caller is expected
  // to verify that any constraint space size (available size, percentage size,
  // and so on) and BFC offset changes won't require re-layout, before skipping.
  bool MaySkipLayout(const NGConstraintSpace& other) const {
    if (HasRareData() && other.HasRareData()) {
      if (!rare_data_->MaySkipLayout(*other.rare_data_))
        return false;
    } else if (!HasRareData() && !other.HasRareData()) {
      if (bfc_offset_.line_offset != other.bfc_offset_.line_offset)
        return false;
    } else {
      // We have a bfc_offset_, and a rare_data_ (or vice-versa).
      return false;
    }

    return exclusion_space_ == other.exclusion_space_ &&
           bitfields_.MaySkipLayout(other.bitfields_);
  }

  bool AreSizesEqual(const NGConstraintSpace& other) const {
    if (available_size_ != other.available_size_)
      return false;

    if (bitfields_.percentage_inline_storage !=
        other.bitfields_.percentage_inline_storage)
      return false;

    if (bitfields_.percentage_block_storage !=
        other.bitfields_.percentage_block_storage)
      return false;

    if (bitfields_.replaced_percentage_inline_storage !=
        other.bitfields_.replaced_percentage_inline_storage)
      return false;

    if (bitfields_.replaced_percentage_block_storage !=
        other.bitfields_.replaced_percentage_block_storage)
      return false;

    // The rest of this method just checks the percentage resolution sizes. If
    // neither space has rare data, we know that they must equal now.
    if (!HasRareData() && !other.HasRareData())
      return true;

    if (bitfields_.percentage_inline_storage == kRareDataPercentage &&
        other.bitfields_.percentage_inline_storage == kRareDataPercentage &&
        rare_data_->percentage_resolution_size.inline_size !=
            other.rare_data_->percentage_resolution_size.inline_size)
      return false;

    if (bitfields_.percentage_block_storage == kRareDataPercentage &&
        other.bitfields_.percentage_block_storage == kRareDataPercentage &&
        rare_data_->percentage_resolution_size.block_size !=
            other.rare_data_->percentage_resolution_size.block_size)
      return false;

    if (bitfields_.replaced_percentage_inline_storage == kRareDataPercentage &&
        other.bitfields_.replaced_percentage_inline_storage ==
            kRareDataPercentage &&
        rare_data_->replaced_percentage_resolution_size.inline_size !=
            other.rare_data_->replaced_percentage_resolution_size.inline_size)
      return false;

    if (bitfields_.replaced_percentage_block_storage == kRareDataPercentage &&
        other.bitfields_.replaced_percentage_block_storage ==
            kRareDataPercentage &&
        rare_data_->replaced_percentage_resolution_size.block_size !=
            other.rare_data_->replaced_percentage_resolution_size.block_size)
      return false;

    return true;
  }
  bool operator==(const NGConstraintSpace&) const;
  bool operator!=(const NGConstraintSpace& other) const {
    return !(*this == other);
  }

  String ToString() const;

 private:
  friend class NGConstraintSpaceBuilder;

  explicit NGConstraintSpace(WritingMode writing_mode)
      : bfc_offset_(), bitfields_(writing_mode) {}

  // This struct defines all of the inputs to layout which we consider rare.
  // Primarily this is:
  //  - Percentage resolution sizes which differ from the available size or
  //    aren't indefinite.
  //  - The margin strut.
  //  - Anything to do with floats (the exclusion space, clearance offset, etc).
  //  - Anything to do with fragmentation.
  //
  // This information is kept in a separate in this heap-allocated struct to
  // reduce memory usage. Over time this may have to change based on usage data.
  struct RareData {
    USING_FAST_MALLOC(RareData);

   public:
    explicit RareData(const NGBfcOffset bfc_offset)
        : bfc_offset(bfc_offset),
          block_direction_fragmentation_type(
              static_cast<unsigned>(kFragmentNone)) {}
    RareData(const RareData&) = default;
    ~RareData() = default;

    NGLogicalSize percentage_resolution_size;
    NGLogicalSize replaced_percentage_resolution_size;

    NGBfcOffset bfc_offset;
    NGMarginStrut margin_strut;

    base::Optional<LayoutUnit> floats_bfc_block_offset;
    LayoutUnit clearance_offset = LayoutUnit::Min();

    LayoutUnit fragmentainer_block_size = NGSizeIndefinite;
    LayoutUnit fragmentainer_space_at_bfc_start = NGSizeIndefinite;

    unsigned block_direction_fragmentation_type : 2;

    bool MaySkipLayout(const RareData& other) const {
      return margin_strut == other.margin_strut &&
             bfc_offset.line_offset == other.bfc_offset.line_offset &&
             floats_bfc_block_offset == other.floats_bfc_block_offset &&
             clearance_offset == other.clearance_offset &&
             fragmentainer_block_size == other.fragmentainer_block_size &&
             fragmentainer_space_at_bfc_start ==
                 other.fragmentainer_space_at_bfc_start &&
             block_direction_fragmentation_type ==
                 other.block_direction_fragmentation_type;
    }
  };

  // This struct simply allows us easily copy, compare, and initialize all the
  // bitfields without having to explicitly copy, compare, and initialize each
  // one (see the outer class constructors, and assignment operators).
  struct Bitfields {
    DISALLOW_NEW();

   public:
    // We explicitly define a default constructor to ensure the kHasRareData
    // bitfield doesn't accidently get set.
    Bitfields() : Bitfields(WritingMode::kHorizontalTb) {}

    explicit Bitfields(WritingMode writing_mode)
        : table_cell_child_layout_phase(
              static_cast<unsigned>(kNotTableCellChild)),
          adjoining_floats(static_cast<unsigned>(kFloatTypeNone)),
          writing_mode(static_cast<unsigned>(writing_mode)),
          direction(static_cast<unsigned>(TextDirection::kLtr)),
          flags(kFixedSizeBlockIsDefinite),
          percentage_inline_storage(kSameAsAvailable),
          percentage_block_storage(kSameAsAvailable),
          replaced_percentage_inline_storage(kSameAsAvailable),
          replaced_percentage_block_storage(kSameAsAvailable) {}

    bool MaySkipLayout(const Bitfields& other) const {
      return table_cell_child_layout_phase ==
                 other.table_cell_child_layout_phase &&
             adjoining_floats == other.adjoining_floats &&
             writing_mode == other.writing_mode && flags == other.flags &&
             baseline_requests == other.baseline_requests;
    }

    unsigned table_cell_child_layout_phase : 2;  // NGTableCellChildLayoutPhase
    unsigned adjoining_floats : 2;               // NGFloatTypes
    unsigned writing_mode : 3;
    unsigned direction : 1;
    unsigned flags : kNumberOfConstraintSpaceFlags;  // ConstraintSpaceFlags
    unsigned baseline_requests : NGBaselineRequestList::kSerializedBits;

    unsigned percentage_inline_storage : 2;           // NGPercentageStorage
    unsigned percentage_block_storage : 2;            // NGPercentageStorage
    unsigned replaced_percentage_inline_storage : 2;  // NGPercentageStorage
    unsigned replaced_percentage_block_storage : 2;   // NGPercentageStorage
  };

  inline bool HasFlag(ConstraintSpaceFlags mask) const {
    return bitfields_.flags & static_cast<unsigned>(mask);
  }

  inline bool HasRareData() const { return HasFlag(kHasRareData); }

  RareData* EnsureRareData() {
    if (!HasRareData()) {
      rare_data_ = new RareData(bfc_offset_);
      bitfields_.flags |= kHasRareData;
    }

    return rare_data_;
  }

  NGLogicalSize available_size_;

  // To save a little space, we union these two fields. rare_data_ is valid if
  // the kHasRareData bitfield is set, otherwise bfc_offset_ is valid.
  union {
    NGBfcOffset bfc_offset_;
    RareData* rare_data_;
  };

  NGExclusionSpace exclusion_space_;
  Bitfields bitfields_;
};

inline std::ostream& operator<<(std::ostream& stream,
                                const NGConstraintSpace& value) {
  return stream << value.ToString();
}

}  // namespace blink

#endif  // NGConstraintSpace_h
