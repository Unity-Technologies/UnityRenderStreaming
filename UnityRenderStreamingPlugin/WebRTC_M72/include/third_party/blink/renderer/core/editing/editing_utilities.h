/*
 * Copyright (C) 2004, 2006, 2008 Apple Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_EDITING_UTILITIES_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_EDITING_UTILITIES_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/editing/editing_boundary.h"
#include "third_party/blink/renderer/core/editing/forward.h"
#include "third_party/blink/renderer/core/events/input_event.h"
#include "third_party/blink/renderer/platform/geometry/float_quad.h"
#include "third_party/blink/renderer/platform/text/text_direction.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/text/character_names.h"

namespace blink {

enum class PositionMoveType {
  // Move by a single code unit. |PositionMoveType::CodeUnit| is used for
  // implementing other |PositionMoveType|. You should not use this.
  kCodeUnit,
  // Move to the next Unicode code point. At most two code unit when we are
  // at surrogate pair. Please consider using |GraphemeCluster|.
  kBackwardDeletion,
  // Move by a grapheme cluster for user-perceived character in Unicode
  // Standard Annex #29, Unicode text segmentation[1].
  // [1] http://www.unicode.org/reports/tr29/
  kGraphemeCluster,
};

class Document;
class Element;
class HTMLElement;
class HTMLSpanElement;
struct LocalCaretRect;
class Node;

// This file contains a set of helper functions used by the editing commands

bool NeedsLayoutTreeUpdate(const Node&);
CORE_EXPORT bool NeedsLayoutTreeUpdate(const Position&);
CORE_EXPORT bool NeedsLayoutTreeUpdate(const PositionInFlatTree&);

// -------------------------------------------------------------------------
// Node
// -------------------------------------------------------------------------

// Returns true if |node| has "user-select:contain".
bool IsUserSelectContain(const Node& /* node */);

CORE_EXPORT bool HasEditableStyle(const Node&);
CORE_EXPORT bool HasRichlyEditableStyle(const Node&);
CORE_EXPORT bool IsRootEditableElement(const Node&);
CORE_EXPORT Element* RootEditableElement(const Node&);
Element* RootEditableElementOf(const Position&);
Element* RootEditableElementOf(const PositionInFlatTree&);
ContainerNode* RootEditableElementOrTreeScopeRootNodeOf(const Position&);
// highestEditableRoot returns the highest editable node. If the
// rootEditableElement of the speicified Position is <body>, this returns the
// <body>. Otherwise, this searches ancestors for the highest editable node in
// defiance of editing boundaries. This returns a Document if designMode="on"
// and the specified Position is not in the <body>.
CORE_EXPORT ContainerNode* HighestEditableRoot(
    const Position&,
    Element* (*)(const Position&) = RootEditableElementOf,
    bool (*)(const Node&) = HasEditableStyle);
ContainerNode* HighestEditableRoot(const PositionInFlatTree&);

Node* HighestEnclosingNodeOfType(
    const Position&,
    bool (*node_is_of_type)(const Node*),
    EditingBoundaryCrossingRule = kCannotCrossEditingBoundary,
    Node* stay_within = nullptr);

Element* EnclosingBlock(
    const Node*,
    EditingBoundaryCrossingRule = kCannotCrossEditingBoundary);
CORE_EXPORT Element* EnclosingBlock(const Position&,
                                    EditingBoundaryCrossingRule);
CORE_EXPORT Element* EnclosingBlock(const PositionInFlatTree&,
                                    EditingBoundaryCrossingRule);
Element* EnclosingBlockFlowElement(
    const Node&);  // Deprecated, use enclosingBlock instead.
Element* AssociatedElementOf(const Position&);
Element* EnclosingAnchorElement(const Position&);
// Returns the lowest ancestor with the specified QualifiedName. If the
// specified Position is editable, this function returns an editable
// Element. Otherwise, editability doesn't matter.
Element* EnclosingElementWithTag(const Position&, const QualifiedName&);
CORE_EXPORT Node* EnclosingNodeOfType(
    const Position&,
    bool (*node_is_of_type)(const Node*),
    EditingBoundaryCrossingRule = kCannotCrossEditingBoundary);
CORE_EXPORT Node* EnclosingNodeOfType(
    const PositionInFlatTree&,
    bool (*node_is_of_type)(const Node*),
    EditingBoundaryCrossingRule = kCannotCrossEditingBoundary);

HTMLSpanElement* TabSpanElement(const Node*);
Element* TableElementJustAfter(const VisiblePosition&);
CORE_EXPORT Element* TableElementJustBefore(const VisiblePosition&);
CORE_EXPORT Element* TableElementJustBefore(const VisiblePositionInFlatTree&);

template <typename Strategy>
ContainerNode* ParentCrossingShadowBoundaries(const Node&);
template <>
inline ContainerNode* ParentCrossingShadowBoundaries<EditingStrategy>(
    const Node& node) {
  return NodeTraversal::ParentOrShadowHostNode(node);
}
template <>
inline ContainerNode* ParentCrossingShadowBoundaries<EditingInFlatTreeStrategy>(
    const Node& node) {
  return FlatTreeTraversal::Parent(node);
}

void WriteImageNodeToClipboard(const Node&, const String&);

// boolean functions on Node

// FIXME: editingIgnoresContent, canHaveChildrenForEditing, and isAtomicNode
// should be renamed to reflect its usage.

// Returns true for nodes that either have no content, or have content that is
// ignored (skipped over) while editing. There are no VisiblePositions inside
// these nodes.
bool EditingIgnoresContent(const Node&);

inline bool CanHaveChildrenForEditing(const Node* node) {
  return !node->IsTextNode() && node->CanContainRangeEndPoint();
}

bool IsAtomicNode(const Node*);
CORE_EXPORT bool IsEnclosingBlock(const Node*);
CORE_EXPORT bool IsTabHTMLSpanElement(const Node*);
bool IsTabHTMLSpanElementTextNode(const Node*);
bool IsMailHTMLBlockquoteElement(const Node*);
// Returns true if the specified node is visible <table>. We don't want to add
// invalid nodes to <table> elements.
bool IsDisplayInsideTable(const Node*);
bool IsTableCell(const Node*);
bool IsHTMLListElement(const Node*);
bool IsListItem(const Node*);
bool IsPresentationalHTMLElement(const Node*);
bool IsRenderedAsNonInlineTableImageOrHR(const Node*);
bool IsNonTableCellHTMLBlockElement(const Node*);
bool IsBlockFlowElement(const Node&);
EUserSelect UsedValueOfUserSelect(const Node&);
bool IsInPasswordField(const Position&);
CORE_EXPORT TextDirection DirectionOfEnclosingBlockOf(const Position&);
CORE_EXPORT TextDirection
DirectionOfEnclosingBlockOf(const PositionInFlatTree&);
CORE_EXPORT TextDirection PrimaryDirectionOf(const Node&);

// -------------------------------------------------------------------------
// Position
// -------------------------------------------------------------------------

// Functions returning Position

Position NextCandidate(const Position&);
PositionInFlatTree NextCandidate(const PositionInFlatTree&);
Position PreviousCandidate(const Position&);
PositionInFlatTree PreviousCandidate(const PositionInFlatTree&);

CORE_EXPORT Position NextVisuallyDistinctCandidate(const Position&);
CORE_EXPORT PositionInFlatTree
NextVisuallyDistinctCandidate(const PositionInFlatTree&);
Position PreviousVisuallyDistinctCandidate(const Position&);
PositionInFlatTree PreviousVisuallyDistinctCandidate(const PositionInFlatTree&);

// This is a |const Node&| versions of two deprecated functions above.
inline Position FirstPositionInOrBeforeNode(const Node& node) {
  return Position::FirstPositionInOrBeforeNode(node);
}

inline Position LastPositionInOrAfterNode(const Node& node) {
  return Position::LastPositionInOrAfterNode(node);
}

CORE_EXPORT Position FirstEditablePositionAfterPositionInRoot(const Position&,
                                                              const Node&);
CORE_EXPORT Position LastEditablePositionBeforePositionInRoot(const Position&,
                                                              const Node&);
CORE_EXPORT PositionInFlatTree
FirstEditablePositionAfterPositionInRoot(const PositionInFlatTree&,
                                         const Node&);
CORE_EXPORT PositionInFlatTree
LastEditablePositionBeforePositionInRoot(const PositionInFlatTree&,
                                         const Node&);

// Move up or down the DOM by one position.
// Offsets are computed using layout text for nodes that have layoutObjects -
// but note that even when using composed characters, the result may be inside
// a single user-visible character if a ligature is formed.
CORE_EXPORT Position PreviousPositionOf(const Position&, PositionMoveType);
CORE_EXPORT Position NextPositionOf(const Position&, PositionMoveType);
CORE_EXPORT PositionInFlatTree PreviousPositionOf(const PositionInFlatTree&,
                                                  PositionMoveType);
CORE_EXPORT PositionInFlatTree NextPositionOf(const PositionInFlatTree&,
                                              PositionMoveType);

CORE_EXPORT int PreviousGraphemeBoundaryOf(const Node&, int current);
CORE_EXPORT int NextGraphemeBoundaryOf(const Node&, int current);

// comparision functions on Position

// |disconnected| is optional output parameter having true if specified
// positions don't have common ancestor.
int ComparePositionsInDOMTree(const Node* container_a,
                              int offset_a,
                              const Node* container_b,
                              int offset_b,
                              bool* disconnected = nullptr);
int ComparePositionsInFlatTree(const Node* container_a,
                               int offset_a,
                               const Node* container_b,
                               int offset_b,
                               bool* disconnected = nullptr);
// TODO(yosin): We replace |comparePositions()| by |Position::opeator<()| to
// utilize |DCHECK_XX()|.
int ComparePositions(const Position&, const Position&);
int ComparePositions(const PositionWithAffinity&, const PositionWithAffinity&);
bool IsNodeFullyContained(const EphemeralRange&, const Node&);

// boolean functions on Position

// FIXME: Both isEditablePosition and isRichlyEditablePosition rely on
// up-to-date style to give proper results. They shouldn't update style by
// default, but should make it clear that that is the contract.
CORE_EXPORT bool IsEditablePosition(const Position&);
bool IsEditablePosition(const PositionInFlatTree&);
bool IsRichlyEditablePosition(const Position&);

PositionWithAffinity PositionRespectingEditingBoundary(
    const Position&,
    const LayoutPoint& local_point,
    Node* target_node);
Position ComputePositionForNodeRemoval(const Position&, const Node&);

// TODO(editing-dev): These two functions should be eliminated.
CORE_EXPORT Position PositionBeforeNode(const Node&);
Position PositionAfterNode(const Node&);

// -------------------------------------------------------------------------
// VisiblePosition
// -------------------------------------------------------------------------

int ComparePositions(const VisiblePosition&, const VisiblePosition&);

CORE_EXPORT int IndexForVisiblePosition(const VisiblePosition&,
                                        ContainerNode*& scope);
EphemeralRange MakeRange(const VisiblePosition&, const VisiblePosition&);
EphemeralRange NormalizeRange(const EphemeralRange&);
EphemeralRangeInFlatTree NormalizeRange(const EphemeralRangeInFlatTree&);
CORE_EXPORT VisiblePosition VisiblePositionForIndex(int index,
                                                    ContainerNode* scope);

// -------------------------------------------------------------------------
// HTMLElement
// -------------------------------------------------------------------------

// Functions returning HTMLElement

HTMLElement* CreateDefaultParagraphElement(Document&);

// -------------------------------------------------------------------------
// Element
// -------------------------------------------------------------------------

// Functions returning Element

HTMLSpanElement* CreateTabSpanElement(Document&);
HTMLSpanElement* CreateTabSpanElement(Document&, const String& tab_text);

Element* FindEventTargetFrom(LocalFrame&, const VisibleSelection&);

// Note: ImageElementFromImageDocument() is both used in ExecuteCopy() and
// Editor::CanCopy()
HTMLImageElement* ImageElementFromImageDocument(const Document*);

// Boolean functions on Element

CORE_EXPORT bool ElementCannotHaveEndTag(const Node&);

// -------------------------------------------------------------------------
// VisibleSelection
// -------------------------------------------------------------------------

// Miscellaneous functions on Text
inline bool IsWhitespace(UChar c) {
  return c == kNoBreakSpaceCharacter || c == ' ' || c == '\n' || c == '\t';
}

// FIXME: Can't really answer this question correctly without knowing the
// white-space mode.
inline bool IsCollapsibleWhitespace(UChar c) {
  return c == ' ' || c == '\n';
}

String StringWithRebalancedWhitespace(const String&,
                                      bool start_is_start_of_paragraph,
                                      bool should_emit_nbs_pbefore_end);

CORE_EXPORT String RepeatString(const String&, unsigned);

// -------------------------------------------------------------------------
// Distance calculation functions
// -------------------------------------------------------------------------

// If current position is at grapheme boundary, return 0; otherwise, return the
// distance to its nearest left grapheme boundary.
wtf_size_t ComputeDistanceToLeftGraphemeBoundary(const Position&);

// If current position is at grapheme boundary, return 0; otherwise, return the
// distance to its nearest right grapheme boundary.
wtf_size_t ComputeDistanceToRightGraphemeBoundary(const Position&);

// -------------------------------------------------------------------------
// LocalCaretRect conversions
// -------------------------------------------------------------------------

FloatQuad LocalToAbsoluteQuadOf(const LocalCaretRect&);

// -------------------------------------------------------------------------
// Events
// -------------------------------------------------------------------------

// Functions dispatch InputEvent
const StaticRangeVector* TargetRangesForInputEvent(const Node&);
DispatchEventResult DispatchBeforeInputInsertText(
    Node*,
    const String& data,
    InputEvent::InputType = InputEvent::InputType::kInsertText,
    const StaticRangeVector* = nullptr);
DispatchEventResult DispatchBeforeInputEditorCommand(Node*,
                                                     InputEvent::InputType,
                                                     const StaticRangeVector*);
DispatchEventResult DispatchBeforeInputDataTransfer(Node*,
                                                    InputEvent::InputType,
                                                    DataTransfer*);
}  // namespace blink

#endif
