/*
 * Copyright (C) 2004 Apple Computer, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_VISIBLE_SELECTION_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_VISIBLE_SELECTION_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/editing/editing_strategy.h"
#include "third_party/blink/renderer/core/editing/forward.h"
#include "third_party/blink/renderer/core/editing/position.h"
#include "third_party/blink/renderer/core/editing/selection_type.h"
#include "third_party/blink/renderer/core/editing/text_affinity.h"
#include "third_party/blink/renderer/core/editing/text_granularity.h"
#include "third_party/blink/renderer/core/editing/visible_units.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace blink {

const TextAffinity kSelDefaultAffinity = TextAffinity::kDownstream;

template <typename Strategy>
class VisibleSelectionTemplate {
  DISALLOW_NEW();

 public:
  class Creator;

  VisibleSelectionTemplate();
  VisibleSelectionTemplate(const VisibleSelectionTemplate&);
  VisibleSelectionTemplate& operator=(const VisibleSelectionTemplate&);

  TextAffinity Affinity() const { return affinity_; }

  SelectionTemplate<Strategy> AsSelection() const;
  PositionTemplate<Strategy> Base() const { return base_; }
  PositionTemplate<Strategy> Extent() const { return extent_; }
  PositionTemplate<Strategy> Start() const;
  PositionTemplate<Strategy> End() const;

  VisiblePositionTemplate<Strategy> VisibleStart() const;
  VisiblePositionTemplate<Strategy> VisibleEnd() const;
  VisiblePositionTemplate<Strategy> VisibleBase() const;
  VisiblePositionTemplate<Strategy> VisibleExtent() const;

  bool operator==(const VisibleSelectionTemplate&) const;
  bool operator!=(const VisibleSelectionTemplate& other) const {
    return !operator==(other);
  }

  bool IsNone() const;
  bool IsCaret() const;
  bool IsRange() const;

  // True if base() <= extent().
  bool IsBaseFirst() const { return base_is_first_; }

  // TODO(yosin) Most callers probably don't want these functions, but
  // are using them for historical reasons. |toNormalizedEphemeralRange()|
  // contracts the range around text, and moves the caret most backward
  // visually equivalent position before returning the range/positions.
  EphemeralRangeTemplate<Strategy> ToNormalizedEphemeralRange() const;

  Element* RootEditableElement() const;
  bool IsContentEditable() const;

  bool IsValidFor(const Document&) const;

  // TODO(editing-dev): |CreateWithoutValidationDeprecated()| is allowed
  // only to use in |TypingCommand| to remove part of grapheme cluster.
  // Note: |base| and |extent| can be disconnect position.
  static VisibleSelectionTemplate<Strategy> CreateWithoutValidationDeprecated(
      const PositionTemplate<Strategy>& base,
      const PositionTemplate<Strategy>& extent,
      TextAffinity);

  void Trace(blink::Visitor*);

#ifndef NDEBUG
  void ShowTreeForThis() const;
#endif
  static void PrintTo(const VisibleSelectionTemplate&, std::ostream*);

 private:
  explicit VisibleSelectionTemplate(const SelectionTemplate<Strategy>&);

  // We need to store these as Positions because VisibleSelection is
  // used to store values in editing commands for use when
  // undoing the command. We need to be able to create a selection that, while
  // currently invalid, will be valid once the changes are undone.

  // Where the first click happened
  PositionTemplate<Strategy> base_;
  // Where the end click happened
  PositionTemplate<Strategy> extent_;

  TextAffinity affinity_;  // the upstream/downstream affinity of the caret

  // these are cached, can be recalculated by validate()
  bool base_is_first_ : 1;        // True if base is before the extent
};

extern template class CORE_EXTERN_TEMPLATE_EXPORT
    VisibleSelectionTemplate<EditingStrategy>;
extern template class CORE_EXTERN_TEMPLATE_EXPORT
    VisibleSelectionTemplate<EditingInFlatTreeStrategy>;

using VisibleSelection = VisibleSelectionTemplate<EditingStrategy>;
using VisibleSelectionInFlatTree =
    VisibleSelectionTemplate<EditingInFlatTreeStrategy>;

CORE_EXPORT VisibleSelection CreateVisibleSelection(const SelectionInDOMTree&);
CORE_EXPORT VisibleSelectionInFlatTree
CreateVisibleSelection(const SelectionInFlatTree&);

CORE_EXPORT VisibleSelection
CreateVisibleSelectionWithGranularity(const SelectionInDOMTree&,
                                      TextGranularity);

CORE_EXPORT VisibleSelectionInFlatTree
CreateVisibleSelectionWithGranularity(const SelectionInFlatTree&,
                                      TextGranularity);

// We don't yet support multi-range selections, so we only ever have one range
// to return.
CORE_EXPORT EphemeralRange FirstEphemeralRangeOf(const VisibleSelection&);

CORE_EXPORT std::ostream& operator<<(std::ostream&, const VisibleSelection&);
CORE_EXPORT std::ostream& operator<<(std::ostream&,
                                     const VisibleSelectionInFlatTree&);

PositionInFlatTree ComputeStartRespectingGranularity(
    const PositionInFlatTreeWithAffinity&,
    TextGranularity);

PositionInFlatTree ComputeEndRespectingGranularity(
    const PositionInFlatTree&,
    const PositionInFlatTreeWithAffinity&,
    TextGranularity);

}  // namespace blink

#ifndef NDEBUG
// Outside the WebCore namespace for ease of invocation from gdb.
void showTree(const blink::VisibleSelection&);
void showTree(const blink::VisibleSelection*);
void showTree(const blink::VisibleSelectionInFlatTree&);
void showTree(const blink::VisibleSelectionInFlatTree*);
#endif

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_VISIBLE_SELECTION_H_
