/*
 * (C) 1999 Lars Knoll (knoll@kde.org)
 * (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004-2009, 2013 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_LAYOUT_TEXT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_LAYOUT_TEXT_H_

#include <iterator>
#include "base/memory/scoped_refptr.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/text.h"
#include "third_party/blink/renderer/core/layout/layout_object.h"
#include "third_party/blink/renderer/core/layout/line/line_box_list.h"
#include "third_party/blink/renderer/core/layout/text_run_constructor.h"
#include "third_party/blink/renderer/platform/geometry/length_functions.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

namespace blink {

class AbstractInlineTextBox;
class InlineTextBox;
class NGInlineItem;
class NGInlineItems;
class NGOffsetMapping;

enum class OnlyWhitespaceOrNbsp : unsigned { kUnknown = 0, kNo = 1, kYes = 2 };

// LayoutText is the root class for anything that represents
// a text node (see core/dom/Text.h).
//
// This is a common node in the tree so to the limit memory overhead,
// this class inherits directly from LayoutObject.
// Also this class is used by both CSS and SVG layouts so LayoutObject
// was a natural choice.
//
// The actual layout of text is handled by the containing inline
// (LayoutInline) or block (LayoutBlockFlow). They will invoke the Unicode
// Bidirectional Algorithm to break the text into actual lines.
// The result of layout is the line box tree, which represents lines
// on the screen. It is stored into m_firstTextBox and m_lastTextBox.
// To understand how lines are broken by the bidi algorithm, read e.g.
// LayoutBlockFlow::layoutInlineChildren.
//
//
// ***** LINE BOXES OWNERSHIP *****
// InlineTextBox in text_boxes_ are not owned by LayoutText but are pointers
// into the enclosing inline / block (see LayoutInline's and LayoutBlockFlow's
// line_boxes_).
//
// This class implements the preferred logical widths computation
// for its underlying text. The widths are stored into m_minWidth
// and m_maxWidth. They are computed lazily based on
// m_preferredLogicalWidthsDirty.
//
// The previous comment applies also for painting. See e.g.
// BlockFlowPainter::paintContents in particular the use of LineBoxListPainter.
class CORE_EXPORT LayoutText : public LayoutObject {
 public:
  // FIXME: If the node argument is not a Text node or the string argument is
  // not the content of the Text node, updating text-transform property
  // doesn't re-transform the string.
  LayoutText(Node*, scoped_refptr<StringImpl>);

  ~LayoutText() override;

  static LayoutText* CreateEmptyAnonymous(Document&,
                                          scoped_refptr<ComputedStyle>);

  const char* GetName() const override { return "LayoutText"; }

  virtual bool IsTextFragment() const;
  virtual bool IsWordBreak() const;

  virtual scoped_refptr<StringImpl> OriginalText() const;

  void ExtractTextBox(InlineTextBox*);
  void AttachTextBox(InlineTextBox*);
  void RemoveTextBox(InlineTextBox*);

  NGPaintFragment* FirstInlineFragment() const final;
  void SetFirstInlineFragment(NGPaintFragment*) final;

  const String& GetText() const { return text_; }
  virtual unsigned TextStartOffset() const { return 0; }
  String PlainText() const;

  // Returns first letter part of |LayoutTextFragment|.
  virtual LayoutText* GetFirstLetterPart() const { return nullptr; }

  InlineTextBox* CreateInlineTextBox(int start, unsigned short length);
  void DirtyOrDeleteLineBoxesIfNeeded(bool full_layout);
  void DirtyLineBoxes();

  void AbsoluteRects(Vector<IntRect>&,
                     const LayoutPoint& accumulated_offset) const final;

  void AbsoluteQuads(Vector<FloatQuad>&,
                     MapCoordinatesFlags mode = 0) const final;
  void AbsoluteQuadsForRange(Vector<FloatQuad>&,
                             unsigned start_offset = 0,
                             unsigned end_offset = INT_MAX) const;
  FloatRect LocalBoundingBoxRectForAccessibility() const final;

  enum ClippingOption { kNoClipping, kClipToEllipsis };
  enum LocalOrAbsoluteOption { kLocalQuads, kAbsoluteQuads };
  void Quads(Vector<FloatQuad>&,
             ClippingOption = kNoClipping,
             LocalOrAbsoluteOption = kAbsoluteQuads,
             MapCoordinatesFlags mode = 0) const;

  PositionWithAffinity PositionForPoint(const LayoutPoint&) const override;

  bool Is8Bit() const { return text_.Is8Bit(); }
  const LChar* Characters8() const { return text_.Impl()->Characters8(); }
  const UChar* Characters16() const { return text_.Impl()->Characters16(); }
  bool HasEmptyText() const { return text_.IsEmpty(); }
  UChar CharacterAt(unsigned) const;
  UChar UncheckedCharacterAt(unsigned) const;
  UChar operator[](unsigned i) const { return UncheckedCharacterAt(i); }
  UChar32 CodepointAt(unsigned) const;
  unsigned TextLength() const {
    return text_.length();
  }  // non virtual implementation of length()
  bool ContainsOnlyWhitespace(unsigned from, unsigned len) const;

  // Get characters after whitespace collapsing was applied. Returns 0 if there
  // were no characters left. If whitespace collapsing is disabled (i.e.
  // white-space: pre), returns characters without whitespace collapsing.
  UChar32 FirstCharacterAfterWhitespaceCollapsing() const;
  UChar32 LastCharacterAfterWhitespaceCollapsing() const;

  void PositionLineBox(InlineBox*);

  virtual float Width(unsigned from,
                      unsigned len,
                      const Font&,
                      LayoutUnit x_pos,
                      TextDirection,
                      HashSet<const SimpleFontData*>* fallback_fonts = nullptr,
                      FloatRect* glyph_bounds = nullptr,
                      float expansion = 0) const;
  virtual float Width(unsigned from,
                      unsigned len,
                      LayoutUnit x_pos,
                      TextDirection,
                      bool first_line = false,
                      HashSet<const SimpleFontData*>* fallback_fonts = nullptr,
                      FloatRect* glyph_bounds = nullptr,
                      float expansion = 0) const;

  float MinLogicalWidth() const;
  float MaxLogicalWidth() const;

  void TrimmedPrefWidths(LayoutUnit lead_width,
                         LayoutUnit& first_line_min_width,
                         bool& has_breakable_start,
                         LayoutUnit& last_line_min_width,
                         bool& has_breakable_end,
                         bool& has_breakable_char,
                         bool& has_break,
                         LayoutUnit& first_line_max_width,
                         LayoutUnit& last_line_max_width,
                         LayoutUnit& min_width,
                         LayoutUnit& max_width,
                         bool& strip_front_spaces,
                         TextDirection);

  virtual LayoutRect LinesBoundingBox() const;

  // Returns the bounding box of visual overflow rects of all line boxes.
  LayoutRect VisualOverflowRect() const;

  FloatPoint FirstRunOrigin() const;

  virtual void SetText(scoped_refptr<StringImpl>,
                       bool force = false,
                       bool avoid_layout_and_only_paint = false);
  void SetTextWithOffset(scoped_refptr<StringImpl>,
                         unsigned offset,
                         unsigned len,
                         bool force = false);

  // TODO(kojii): setTextInternal() is temporarily public for NGInlineNode.
  // This will be back to protected when NGInlineNode can paint directly.
  virtual void SetTextInternal(scoped_refptr<StringImpl>);

  virtual void TransformText();

  LayoutRect LocalSelectionRect() const final;
  LayoutRect LocalCaretRect(
      const InlineBox*,
      int caret_offset,
      LayoutUnit* extra_width_to_end_of_line = nullptr) const override;

  // TextBoxes() and FirstInlineFragment() are mutually exclusive,
  // depends on IsInLayoutNGInlineFormattingContext().
  const InlineTextBoxList& TextBoxes() const {
    return IsInLayoutNGInlineFormattingContext() ? InlineTextBoxList::Empty()
                                                 : text_boxes_;
  }

  // Returns first |InlineTextBox| produces for associated |Node|.
  // Note: When |this| is remaining part of ::first-letter, this function
  // returns first-letter part of |InlineTextBox| instead of remaining part.
  InlineTextBox* FirstTextBox() const { return TextBoxes().First(); }
  InlineTextBox* LastTextBox() const { return TextBoxes().Last(); }

  // Returns upper left corner point in local physical coordinates with flipped
  // block-flow direction if this object has rendered text.
  base::Optional<FloatPoint> GetUpperLeftCorner() const;

  // True if we have inline text box children which implies rendered text (or
  // whitespace) output.
  bool HasTextBoxes() const;

  // TODO(layoutng) Legacy-only implementation of HasTextBoxes.
  // All callers should call HasTextBoxes instead, and take NG into account.
  bool HasLegacyTextBoxes() const { return FirstTextBox(); }

  // Compute the rect and offset of text boxes for this LayoutText.
  struct TextBoxInfo {
    LayoutRect local_rect;
    unsigned dom_start_offset;
    unsigned dom_length;
  };
  Vector<TextBoxInfo> GetTextBoxInfo() const;

  // Returns the Position in DOM that corresponds to the given offset in the
  // |text_| string.
  // TODO(layout-dev): Fix it when text-transform changes text length.
  virtual Position PositionForCaretOffset(unsigned) const;

  // Returns the offset in the |text_| string that corresponds to the given
  // position in DOM; Returns nullopt is the position is not in this LayoutText.
  // TODO(layout-dev): Fix it when text-transform changes text length.
  virtual base::Optional<unsigned> CaretOffsetForPosition(
      const Position&) const;

  // Returns true if the offset (0-based in the |text_| string) is next to a
  // non-collapsed non-linebreak character, or before a forced linebreak (<br>,
  // or segment break in node with style white-space: pre/pre-line/pre-wrap).
  // TODO(editing-dev): The behavior is introduced by crrev.com/e3eb4e in
  // InlineTextBox::ContainsCaretOffset(). Try to understand it.
  bool ContainsCaretOffset(int) const;

  // Return true if the offset (0-based in the |text_| string) is before/after a
  // non-collapsed character in this LayoutText, respectively.
  bool IsBeforeNonCollapsedCharacter(unsigned) const;
  bool IsAfterNonCollapsedCharacter(unsigned) const;

  int CaretMinOffset() const override;
  int CaretMaxOffset() const override;
  unsigned ResolvedTextLength() const;

  // True if any character remains after CSS white-space collapsing.
  bool HasNonCollapsedText() const;

  bool ContainsReversedText() const { return contains_reversed_text_; }

  bool IsSecure() const {
    return StyleRef().TextSecurity() != ETextSecurity::kNone;
  }
  void MomentarilyRevealLastTypedCharacter(
      unsigned last_typed_character_offset);

  bool IsAllCollapsibleWhitespace() const;

  void RemoveAndDestroyTextBoxes();

  scoped_refptr<AbstractInlineTextBox> FirstAbstractInlineTextBox();

  float HyphenWidth(const Font&, TextDirection);

  LayoutRect DebugRect() const override;

  void AutosizingMultiplerChanged() {
    known_to_have_no_overflow_and_no_fallback_fonts_ = false;

    // The font size is changing, so we need to make sure to rebuild everything.
    valid_ng_items_ = false;
  }

  OnlyWhitespaceOrNbsp ContainsOnlyWhitespaceOrNbsp() const;

  virtual UChar PreviousCharacter() const;

  // Returns the NGOffsetMapping object when the current text is laid out with
  // LayoutNG.
  // Note that the text can be in legacy layout even when LayoutNG is enabled,
  // so we can't simply check the RuntimeEnabledFeature.
  const NGOffsetMapping* GetNGOffsetMapping() const;

  // Map DOM offset to LayoutNG text content offset.
  // Returns false if all characters in this LayoutText are collapsed.
  bool MapDOMOffsetToTextContentOffset(const NGOffsetMapping&,
                                       unsigned* start,
                                       unsigned* end) const;

  void AddInlineItem(NGInlineItem* item);
  void ClearInlineItems();
  bool HasValidInlineItems() const { return valid_ng_items_; }
  const Vector<NGInlineItem*>& InlineItems() const;
  // Inline items depends on context. It needs to be invalidated not only when
  // it was inserted/changed but also it was moved.
  void InvalidateInlineItems() { valid_ng_items_ = false; }

 protected:
  virtual const NGInlineItems* GetNGInlineItems() const { return nullptr; }
  virtual NGInlineItems* GetNGInlineItems() { return nullptr; }
  void WillBeDestroyed() override;

  void StyleWillChange(StyleDifference, const ComputedStyle&) final {}
  void StyleDidChange(StyleDifference, const ComputedStyle* old_style) override;

  void InLayoutNGInlineFormattingContextWillChange(bool) final;

  void AddLayerHitTestRects(
      LayerHitTestRects&,
      const PaintLayer* current_layer,
      const LayoutPoint& layer_offset,
      TouchAction supported_fast_actions,
      const LayoutRect& container_rect,
      TouchAction container_whitelisted_touch_action) const override;

  virtual InlineTextBox* CreateTextBox(
      int start,
      unsigned short length);  // Subclassed by SVG.

  void InvalidateDisplayItemClients(PaintInvalidationReason) const override;

  bool CanBeSelectionLeafInternal() const final { return true; }

 private:
  InlineTextBoxList& MutableTextBoxes();

  void AccumlateQuads(Vector<FloatQuad>&,
                      const IntRect& ellipsis_rect,
                      LocalOrAbsoluteOption,
                      MapCoordinatesFlags mode,
                      const LayoutRect&) const;

  void ComputePreferredLogicalWidths(float lead_width);
  void ComputePreferredLogicalWidths(
      float lead_width,
      HashSet<const SimpleFontData*>& fallback_fonts,
      FloatRect& glyph_bounds);

  // Make length() private so that callers that have a LayoutText*
  // will use the more efficient textLength() instead, while
  // callers with a LayoutObject* can continue to use length().
  unsigned length() const final { return TextLength(); }

  // See the class comment as to why we shouldn't call this function directly.
  void Paint(const PaintInfo&) const final { NOTREACHED(); }
  void UpdateLayout() final { NOTREACHED(); }
  bool NodeAtPoint(HitTestResult&,
                   const HitTestLocation&,
                   const LayoutPoint&,
                   HitTestAction) final {
    NOTREACHED();
    return false;
  }

  void DeleteTextBoxes();
  float WidthFromFont(const Font&,
                      int start,
                      int len,
                      float lead_width,
                      float text_width_so_far,
                      TextDirection,
                      HashSet<const SimpleFontData*>* fallback_fonts,
                      FloatRect* glyph_bounds_accumulation,
                      float expansion = 0) const;

  void SecureText(UChar mask);

  bool IsText() const =
      delete;  // This will catch anyone doing an unnecessary check.

  LayoutRect LocalVisualRectIgnoringVisibility() const final;

  bool CanOptimizeSetText() const;
  void SetFirstTextBoxLogicalLeft(float text_width) const;

  // We put the bitfield first to minimize padding on 64-bit.
 protected:
  // Whether or not we can be broken into multiple lines.
  unsigned has_breakable_char_ : 1;
  // Whether or not we have a hard break (e.g., <pre> with '\n').
  unsigned has_break_ : 1;
  // Whether or not we have a variable width tab character (e.g., <pre> with
  // '\t').
  unsigned has_tab_ : 1;
  unsigned has_breakable_start_ : 1;
  unsigned has_breakable_end_ : 1;
  unsigned has_end_white_space_ : 1;
  // This bit indicates that the text run has already dirtied specific line
  // boxes, and this hint will enable layoutInlineChildren to avoid just
  // dirtying everything when character data is modified (e.g., appended/
  // inserted or removed).
  unsigned lines_dirty_ : 1;

  // Used by LayoutNGText. Whether the NGInlineItems associated with this
  // object are valid. Set after layout and cleared whenever the LayoutText is
  // modified.
  // Functionally the inverse equivalent of lines_dirty_ for LayoutNG.
  unsigned valid_ng_items_ : 1;

  unsigned contains_reversed_text_ : 1;
  mutable unsigned known_to_have_no_overflow_and_no_fallback_fonts_ : 1;
  unsigned contains_only_whitespace_or_nbsp_ : 2;

 private:
  // Used for LayoutNG with accessibility. True if inline fragments are
  // associated to |NGAbstractInlineTextBox|.
  unsigned has_abstract_inline_text_box_ : 1;
  float min_width_;
  float max_width_;
  float first_line_min_width_;
  float last_line_line_min_width_;

  String text_;

  union {
    // The line boxes associated with this object.
    // Read the LINE BOXES OWNERSHIP section in the class header comment.
    // Valid only when !IsInLayoutNGInlineFormattingContext().
    InlineTextBoxList text_boxes_;
    // The first fragment of text boxes associated with this object.
    // Valid only when IsInLayoutNGInlineFormattingContext().
    NGPaintFragment* first_paint_fragment_;
  };
};

inline InlineTextBoxList& LayoutText::MutableTextBoxes() {
  CHECK(!IsInLayoutNGInlineFormattingContext());
  return text_boxes_;
}

inline NGPaintFragment* LayoutText::FirstInlineFragment() const {
  return IsInLayoutNGInlineFormattingContext() ? first_paint_fragment_
                                               : nullptr;
}

inline UChar LayoutText::UncheckedCharacterAt(unsigned i) const {
  SECURITY_DCHECK(i < TextLength());
  return Is8Bit() ? Characters8()[i] : Characters16()[i];
}

inline UChar LayoutText::CharacterAt(unsigned i) const {
  if (i >= TextLength())
    return 0;

  return UncheckedCharacterAt(i);
}

inline UChar32 LayoutText::CodepointAt(unsigned i) const {
  if (i >= TextLength())
    return 0;
  if (Is8Bit())
    return Characters8()[i];
  UChar32 c;
  U16_GET(Characters16(), 0, i, TextLength(), c);
  return c;
}

inline float LayoutText::HyphenWidth(const Font& font,
                                     TextDirection direction) {
  const ComputedStyle& style = StyleRef();
  return font.Width(ConstructTextRun(font, style.HyphenString().GetString(),
                                     style, direction));
}

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutText, IsText());

inline LayoutText* Text::GetLayoutObject() const {
  return ToLayoutText(CharacterData::GetLayoutObject());
}

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_LAYOUT_TEXT_H_
