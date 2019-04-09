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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_VISIBLE_UNITS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_VISIBLE_UNITS_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/editing/editing_boundary.h"
#include "third_party/blink/renderer/core/editing/forward.h"
#include "third_party/blink/renderer/platform/geometry/layout_rect.h"
#include "third_party/blink/renderer/platform/text/text_direction.h"
#include "third_party/blink/renderer/platform/wtf/text/unicode.h"

namespace blink {

class LayoutUnit;
class LayoutObject;
class Node;
class IntPoint;
class IntRect;
class LocalFrame;

// |EWordSiste| is used as a parameter of |StartOfWord()| and |EndOfWord()|
// to control a returning position when they are called for a position before
// word boundary.
enum EWordSide {
  kNextWordIfOnBoundary = false,
  kPreviousWordIfOnBoundary = true
};

// offset functions on Node
CORE_EXPORT int CaretMinOffset(const Node*);
CORE_EXPORT int CaretMaxOffset(const Node*);

// Position
// mostForward/BackwardCaretPosition are used for moving back and forth between
// visually equivalent candidates.
// For example, for the text node "foo     bar" where whitespace is
// collapsible, there are two candidates that map to the VisiblePosition
// between 'b' and the space, after first space and before last space.
//
// mostBackwardCaretPosition returns the left candidate and also returs
// [boundary, 0] for any of the positions from [boundary, 0] to the first
// candidate in boundary, where
// endsOfNodeAreVisuallyDistinctPositions(boundary) is true.
//
// mostForwardCaretPosition() returns the right one and also returns the
// last position in the last atomic node in boundary for all of the positions
// in boundary after the last candidate, where
// endsOfNodeAreVisuallyDistinctPositions(boundary).
CORE_EXPORT Position MostBackwardCaretPosition(
    const Position&,
    EditingBoundaryCrossingRule = kCannotCrossEditingBoundary);
CORE_EXPORT PositionInFlatTree MostBackwardCaretPosition(
    const PositionInFlatTree&,
    EditingBoundaryCrossingRule = kCannotCrossEditingBoundary);
CORE_EXPORT Position MostForwardCaretPosition(
    const Position&,
    EditingBoundaryCrossingRule = kCannotCrossEditingBoundary);
CORE_EXPORT PositionInFlatTree MostForwardCaretPosition(
    const PositionInFlatTree&,
    EditingBoundaryCrossingRule = kCannotCrossEditingBoundary);

CORE_EXPORT bool IsVisuallyEquivalentCandidate(const Position&);
CORE_EXPORT bool IsVisuallyEquivalentCandidate(const PositionInFlatTree&);

// Whether or not [node, 0] and [node, lastOffsetForEditing(node)] are their own
// VisiblePositions.
// If true, adjacent candidates are visually distinct.
CORE_EXPORT bool EndsOfNodeAreVisuallyDistinctPositions(const Node*);

CORE_EXPORT Position CanonicalPositionOf(const Position&);
CORE_EXPORT PositionInFlatTree CanonicalPositionOf(const PositionInFlatTree&);

CORE_EXPORT UChar32 CharacterAfter(const VisiblePosition&);
CORE_EXPORT UChar32 CharacterAfter(const VisiblePositionInFlatTree&);
CORE_EXPORT UChar32 CharacterBefore(const VisiblePosition&);
CORE_EXPORT UChar32 CharacterBefore(const VisiblePositionInFlatTree&);

CORE_EXPORT VisiblePosition
NextPositionOf(const VisiblePosition&,
               EditingBoundaryCrossingRule = kCanCrossEditingBoundary);
CORE_EXPORT VisiblePositionInFlatTree
NextPositionOf(const VisiblePositionInFlatTree&,
               EditingBoundaryCrossingRule = kCanCrossEditingBoundary);
CORE_EXPORT VisiblePosition
PreviousPositionOf(const VisiblePosition&,
                   EditingBoundaryCrossingRule = kCanCrossEditingBoundary);
CORE_EXPORT VisiblePositionInFlatTree
PreviousPositionOf(const VisiblePositionInFlatTree&,
                   EditingBoundaryCrossingRule = kCanCrossEditingBoundary);

// words
// TODO(yoichio): Replace |startOfWord| to |startOfWordPosition| because
// returned Position should be canonicalized with |previousBoundary()| by
// TextItetator.
CORE_EXPORT Position StartOfWordPosition(const Position&,
                                         EWordSide = kNextWordIfOnBoundary);
CORE_EXPORT VisiblePosition StartOfWord(const VisiblePosition&,
                                        EWordSide = kNextWordIfOnBoundary);
CORE_EXPORT PositionInFlatTree
StartOfWordPosition(const PositionInFlatTree&,
                    EWordSide = kNextWordIfOnBoundary);
CORE_EXPORT VisiblePositionInFlatTree
StartOfWord(const VisiblePositionInFlatTree&,
            EWordSide = kNextWordIfOnBoundary);
CORE_EXPORT VisiblePosition EndOfWord(const VisiblePosition&,
                                      EWordSide = kNextWordIfOnBoundary);
CORE_EXPORT Position EndOfWordPosition(const Position&,
                                       EWordSide = kNextWordIfOnBoundary);
CORE_EXPORT PositionInFlatTree
EndOfWordPosition(const PositionInFlatTree&, EWordSide = kNextWordIfOnBoundary);
CORE_EXPORT VisiblePositionInFlatTree
EndOfWord(const VisiblePositionInFlatTree&, EWordSide = kNextWordIfOnBoundary);
CORE_EXPORT PositionWithAffinity PreviousWordPosition(const Position&);
CORE_EXPORT PositionWithAffinity NextWordPosition(const Position&);

// sentences
CORE_EXPORT Position StartOfSentencePosition(const Position&);
CORE_EXPORT PositionInFlatTree
StartOfSentencePosition(const PositionInFlatTree&);
CORE_EXPORT VisiblePosition StartOfSentence(const VisiblePosition&);
CORE_EXPORT VisiblePositionInFlatTree
StartOfSentence(const VisiblePositionInFlatTree&);
CORE_EXPORT PositionWithAffinity EndOfSentence(const Position&);
CORE_EXPORT PositionInFlatTreeWithAffinity
EndOfSentence(const PositionInFlatTree&);
CORE_EXPORT VisiblePosition EndOfSentence(const VisiblePosition&);
CORE_EXPORT VisiblePositionInFlatTree
EndOfSentence(const VisiblePositionInFlatTree&);
VisiblePosition PreviousSentencePosition(const VisiblePosition&);
VisiblePosition NextSentencePosition(const VisiblePosition&);
EphemeralRange ExpandEndToSentenceBoundary(const EphemeralRange&);
EphemeralRange ExpandRangeToSentenceBoundary(const EphemeralRange&);

// lines
// TODO(yosin) Return values of |VisiblePosition| version of |startOfLine()|
// with shadow tree isn't defined well. We should not use it for shadow tree.
CORE_EXPORT VisiblePosition StartOfLine(const VisiblePosition&);
CORE_EXPORT VisiblePositionInFlatTree
StartOfLine(const VisiblePositionInFlatTree&);
// TODO(yosin) Return values of |VisiblePosition| version of |endOfLine()| with
// shadow tree isn't defined well. We should not use it for shadow tree.
CORE_EXPORT VisiblePosition EndOfLine(const VisiblePosition&);
CORE_EXPORT VisiblePositionInFlatTree
EndOfLine(const VisiblePositionInFlatTree&);
enum EditableType { kContentIsEditable, kHasEditableAXRole };
CORE_EXPORT VisiblePosition
PreviousLinePosition(const VisiblePosition&,
                     LayoutUnit line_direction_point,
                     EditableType = kContentIsEditable);
CORE_EXPORT VisiblePosition NextLinePosition(const VisiblePosition&,
                                             LayoutUnit line_direction_point,
                                             EditableType = kContentIsEditable);
CORE_EXPORT bool InSameLine(const VisiblePosition&, const VisiblePosition&);
CORE_EXPORT bool InSameLine(const VisiblePositionInFlatTree&,
                            const VisiblePositionInFlatTree&);
CORE_EXPORT bool InSameLine(const PositionWithAffinity&,
                            const PositionWithAffinity&);
CORE_EXPORT bool InSameLine(const PositionInFlatTreeWithAffinity&,
                            const PositionInFlatTreeWithAffinity&);
CORE_EXPORT bool IsStartOfLine(const VisiblePosition&);
CORE_EXPORT bool IsStartOfLine(const VisiblePositionInFlatTree&);
CORE_EXPORT bool IsEndOfLine(const VisiblePosition&);
CORE_EXPORT bool IsEndOfLine(const VisiblePositionInFlatTree&);
// TODO(yosin) Return values of |VisiblePosition| version of
// |logicalStartOfLine()| with shadow tree isn't defined well. We should not use
// it for shadow tree.
CORE_EXPORT VisiblePosition LogicalStartOfLine(const VisiblePosition&);
CORE_EXPORT VisiblePositionInFlatTree
LogicalStartOfLine(const VisiblePositionInFlatTree&);
// TODO(yosin) Return values of |VisiblePosition| version of
// |logicalEndOfLine()| with shadow tree isn't defined well. We should not use
// it for shadow tree.
CORE_EXPORT VisiblePosition LogicalEndOfLine(const VisiblePosition&);
CORE_EXPORT VisiblePositionInFlatTree
LogicalEndOfLine(const VisiblePositionInFlatTree&);
CORE_EXPORT bool IsLogicalEndOfLine(const VisiblePosition&);
CORE_EXPORT bool IsLogicalEndOfLine(const VisiblePositionInFlatTree&);

// paragraphs (perhaps a misnomer, can be divided by line break elements)
// TODO(yosin) Since return value of |startOfParagraph()| with |VisiblePosition|
// isn't defined well on flat tree, we should not use it for a position in
// flat tree.
CORE_EXPORT VisiblePosition
StartOfParagraph(const VisiblePosition&,
                 EditingBoundaryCrossingRule = kCannotCrossEditingBoundary);
CORE_EXPORT VisiblePositionInFlatTree
StartOfParagraph(const VisiblePositionInFlatTree&,
                 EditingBoundaryCrossingRule = kCannotCrossEditingBoundary);
CORE_EXPORT VisiblePosition
EndOfParagraph(const VisiblePosition&,
               EditingBoundaryCrossingRule = kCannotCrossEditingBoundary);
CORE_EXPORT VisiblePositionInFlatTree
EndOfParagraph(const VisiblePositionInFlatTree&,
               EditingBoundaryCrossingRule = kCannotCrossEditingBoundary);
VisiblePosition StartOfNextParagraph(const VisiblePosition&);
CORE_EXPORT bool IsStartOfParagraph(
    const VisiblePosition&,
    EditingBoundaryCrossingRule = kCannotCrossEditingBoundary);
CORE_EXPORT bool IsStartOfParagraph(const VisiblePositionInFlatTree&);
CORE_EXPORT bool IsEndOfParagraph(
    const VisiblePosition&,
    EditingBoundaryCrossingRule = kCannotCrossEditingBoundary);
CORE_EXPORT bool IsEndOfParagraph(const VisiblePositionInFlatTree&);
bool InSameParagraph(const VisiblePosition&,
                     const VisiblePosition&,
                     EditingBoundaryCrossingRule = kCannotCrossEditingBoundary);
EphemeralRange ExpandToParagraphBoundary(const EphemeralRange&);

// document
CORE_EXPORT VisiblePosition StartOfDocument(const VisiblePosition&);
CORE_EXPORT VisiblePositionInFlatTree
StartOfDocument(const VisiblePositionInFlatTree&);
CORE_EXPORT VisiblePosition EndOfDocument(const VisiblePosition&);
CORE_EXPORT VisiblePositionInFlatTree
EndOfDocument(const VisiblePositionInFlatTree&);
bool IsStartOfDocument(const VisiblePosition&);
bool IsEndOfDocument(const VisiblePosition&);

// editable content
VisiblePosition StartOfEditableContent(const VisiblePosition&);
VisiblePosition EndOfEditableContent(const VisiblePosition&);
CORE_EXPORT bool IsEndOfEditableOrNonEditableContent(const VisiblePosition&);
CORE_EXPORT bool IsEndOfEditableOrNonEditableContent(
    const VisiblePositionInFlatTree&);

bool HasRenderedNonAnonymousDescendantsWithHeight(const LayoutObject*);

// TODO(editing-dev): We should move this functions to somewhere else.
// Returns a hit-tested PositionWithAffinity for the given point in
// contents-space coordinates.
CORE_EXPORT PositionWithAffinity
PositionForContentsPointRespectingEditingBoundary(const IntPoint&, LocalFrame*);

CORE_EXPORT bool RendersInDifferentPosition(const Position&, const Position&);

CORE_EXPORT Position SkipWhitespace(const Position&);
CORE_EXPORT PositionInFlatTree SkipWhitespace(const PositionInFlatTree&);

CORE_EXPORT IntRect ComputeTextRect(const EphemeralRange&);
IntRect ComputeTextRect(const EphemeralRangeInFlatTree&);
FloatRect ComputeTextFloatRect(const EphemeralRange&);

// |FirstRectForRange| requires up-to-date layout.
IntRect FirstRectForRange(const EphemeralRange&);

CORE_EXPORT PositionWithAffinity
AdjustForwardPositionToAvoidCrossingEditingBoundaries(
    const PositionWithAffinity&,
    const Position&);

PositionInFlatTreeWithAffinity
AdjustForwardPositionToAvoidCrossingEditingBoundaries(
    const PositionInFlatTreeWithAffinity&,
    const PositionInFlatTree&);

PositionWithAffinity AdjustBackwardPositionToAvoidCrossingEditingBoundaries(
    const PositionWithAffinity&,
    const Position&);

PositionInFlatTreeWithAffinity
AdjustBackwardPositionToAvoidCrossingEditingBoundaries(
    const PositionInFlatTreeWithAffinity&,
    const PositionInFlatTree&);

VisiblePosition AdjustForwardPositionToAvoidCrossingEditingBoundaries(
    const VisiblePosition&,
    const Position&);

VisiblePositionInFlatTree AdjustForwardPositionToAvoidCrossingEditingBoundaries(
    const VisiblePositionInFlatTree&,
    const PositionInFlatTree&);

// Export below functions only for |SelectionModifier|.
VisiblePosition AdjustBackwardPositionToAvoidCrossingEditingBoundaries(
    const VisiblePosition&,
    const Position&);

VisiblePositionInFlatTree
AdjustBackwardPositionToAvoidCrossingEditingBoundaries(
    const VisiblePositionInFlatTree&,
    const PositionInFlatTree&);

Position NextRootInlineBoxCandidatePosition(Node*,
                                            const VisiblePosition&,
                                            EditableType);

CORE_EXPORT Position
PreviousRootInlineBoxCandidatePosition(Node*,
                                       const VisiblePosition&,
                                       EditableType);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_VISIBLE_UNITS_H_
