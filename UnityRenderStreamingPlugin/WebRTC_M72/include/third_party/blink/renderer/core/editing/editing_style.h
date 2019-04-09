/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 * Copyright (C) 2013 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_EDITING_STYLE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_EDITING_STYLE_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/css/css_property_names.h"
#include "third_party/blink/renderer/core/css_value_keywords.h"
#include "third_party/blink/renderer/core/editing/forward.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class CSSStyleDeclaration;
class CSSComputedStyleDeclaration;
class ContainerNode;
class Document;
class Element;
class HTMLElement;
class LocalFrame;
class MutableCSSPropertyValueSet;
class Node;
class QualifiedName;
class ComputedStyle;
class CSSPropertyValueSet;
enum class EditingTriState;
enum class SecureContextMode;
enum class WritingDirection;

class CORE_EXPORT EditingStyle final : public GarbageCollected<EditingStyle> {
 public:
  enum PropertiesToInclude {
    kAllProperties,
    kOnlyEditingInheritableProperties,
    kEditingPropertiesInEffect
  };
  enum ShouldPreserveWritingDirection {
    kPreserveWritingDirection,
    kDoNotPreserveWritingDirection
  };
  enum ShouldExtractMatchingStyle {
    kExtractMatchingStyle,
    kDoNotExtractMatchingStyle
  };
  static float no_font_delta_;

  static EditingStyle* Create() { return MakeGarbageCollected<EditingStyle>(); }

  static EditingStyle* Create(ContainerNode* node,
                              PropertiesToInclude properties_to_include =
                                  kOnlyEditingInheritableProperties) {
    return MakeGarbageCollected<EditingStyle>(node, properties_to_include);
  }

  static EditingStyle* Create(const Position& position,
                              PropertiesToInclude properties_to_include =
                                  kOnlyEditingInheritableProperties) {
    return MakeGarbageCollected<EditingStyle>(position, properties_to_include);
  }

  static EditingStyle* Create(const CSSPropertyValueSet* style) {
    return MakeGarbageCollected<EditingStyle>(style);
  }

  static EditingStyle* Create(CSSPropertyID property_id,
                              const String& value,
                              SecureContextMode secure_context_mode) {
    return MakeGarbageCollected<EditingStyle>(property_id, value,
                                              secure_context_mode);
  }

  EditingStyle() = default;
  EditingStyle(ContainerNode*, PropertiesToInclude);
  EditingStyle(const Position&, PropertiesToInclude);
  explicit EditingStyle(const CSSPropertyValueSet*);
  EditingStyle(CSSPropertyID, const String& value, SecureContextMode);

  MutableCSSPropertyValueSet* Style() { return mutable_style_.Get(); }
  bool GetTextDirection(WritingDirection&) const;
  bool IsEmpty() const;
  void OverrideWithStyle(const CSSPropertyValueSet*);
  void Clear();
  EditingStyle* Copy() const;
  EditingStyle* ExtractAndRemoveBlockProperties();
  EditingStyle* ExtractAndRemoveTextDirection(SecureContextMode);
  void RemoveBlockProperties();
  void RemoveStyleAddedByElement(Element*);
  void RemoveStyleConflictingWithStyleOfElement(Element*);
  void CollapseTextDecorationProperties(SecureContextMode);
  enum ShouldIgnoreTextOnlyProperties {
    kIgnoreTextOnlyProperties,
    kDoNotIgnoreTextOnlyProperties
  };
  EditingTriState TriStateOfStyle(EditingStyle*, SecureContextMode) const;
  EditingTriState TriStateOfStyle(const VisibleSelection&,
                                  SecureContextMode) const;
  bool ConflictsWithInlineStyleOfElement(HTMLElement* element) const {
    return ConflictsWithInlineStyleOfElement(element, nullptr, nullptr);
  }
  bool ConflictsWithInlineStyleOfElement(
      HTMLElement* element,
      EditingStyle* extracted_style,
      Vector<CSSPropertyID>& conflicting_properties) const {
    return ConflictsWithInlineStyleOfElement(element, extracted_style,
                                             &conflicting_properties);
  }
  bool ConflictsWithImplicitStyleOfElement(
      HTMLElement*,
      EditingStyle* extracted_style = nullptr,
      ShouldExtractMatchingStyle = kDoNotExtractMatchingStyle) const;
  bool ConflictsWithImplicitStyleOfAttributes(HTMLElement*) const;
  bool ExtractConflictingImplicitStyleOfAttributes(
      HTMLElement*,
      ShouldPreserveWritingDirection,
      EditingStyle* extracted_style,
      Vector<QualifiedName>& conflicting_attributes,
      ShouldExtractMatchingStyle) const;
  bool StyleIsPresentInComputedStyleOfNode(Node*) const;

  static bool ElementIsStyledSpanOrHTMLEquivalent(const HTMLElement*);

  void PrepareToApplyAt(
      const Position&,
      ShouldPreserveWritingDirection = kDoNotPreserveWritingDirection);
  void MergeTypingStyle(Document*);
  enum CSSPropertyOverrideMode { kOverrideValues, kDoNotOverrideValues };
  void MergeInlineStyleOfElement(HTMLElement*,
                                 CSSPropertyOverrideMode,
                                 PropertiesToInclude = kAllProperties);
  void MergeInlineAndImplicitStyleOfElement(Element*,
                                            CSSPropertyOverrideMode,
                                            PropertiesToInclude);
  void MergeStyleFromRules(Element*);
  void MergeStyleFromRulesForSerialization(Element*);
  void RemoveStyleFromRulesAndContext(Element*, ContainerNode* context);
  void RemovePropertiesInElementDefaultStyle(Element*);
  void ForceInline();
  int LegacyFontSize(Document*) const;

  float FontSizeDelta() const { return font_size_delta_; }
  bool HasFontSizeDelta() const { return font_size_delta_ != no_font_delta_; }

  void SetProperty(CSSPropertyID,
                   const String& value,
                   bool important,
                   SecureContextMode);

  void Trace(blink::Visitor*);
  static EditingTriState SelectionHasStyle(const LocalFrame&,
                                           CSSPropertyID,
                                           const String& value);

 private:
  void Init(Node*, PropertiesToInclude);
  void RemoveInheritedColorsIfNeeded(const ComputedStyle*);
  void ReplaceFontSizeByKeywordIfPossible(const ComputedStyle*,
                                          SecureContextMode,
                                          CSSComputedStyleDeclaration*);
  void ExtractFontSizeDelta();
  EditingTriState TriStateOfStyle(CSSStyleDeclaration* style_to_compare,
                                  ShouldIgnoreTextOnlyProperties,
                                  SecureContextMode) const;
  bool ConflictsWithInlineStyleOfElement(
      HTMLElement*,
      EditingStyle* extracted_style,
      Vector<CSSPropertyID>* conflicting_properties) const;
  void MergeStyle(const CSSPropertyValueSet*, CSSPropertyOverrideMode);

  Member<MutableCSSPropertyValueSet> mutable_style_;
  bool is_monospace_font_ = false;
  float font_size_delta_ = no_font_delta_;
  bool is_vertical_align_ = false;

  friend class HTMLElementEquivalent;
  friend class HTMLAttributeEquivalent;
};

class StyleChange {
  DISALLOW_NEW();

 public:
  StyleChange()
      : apply_bold_(false),
        apply_italic_(false),
        apply_underline_(false),
        apply_line_through_(false),
        apply_subscript_(false),
        apply_superscript_(false) {}

  StyleChange(EditingStyle*, const Position&);

  String CssStyle() const { return css_style_; }
  bool ApplyBold() const { return apply_bold_; }
  bool ApplyItalic() const { return apply_italic_; }
  bool ApplyUnderline() const { return apply_underline_; }
  bool ApplyLineThrough() const { return apply_line_through_; }
  bool ApplySubscript() const { return apply_subscript_; }
  bool ApplySuperscript() const { return apply_superscript_; }
  bool ApplyFontColor() const { return apply_font_color_.length() > 0; }
  bool ApplyFontFace() const { return apply_font_face_.length() > 0; }
  bool ApplyFontSize() const { return apply_font_size_.length() > 0; }

  String FontColor() { return apply_font_color_; }
  String FontFace() { return apply_font_face_; }
  String FontSize() { return apply_font_size_; }

  bool operator==(const StyleChange& other) {
    return css_style_ == other.css_style_ && apply_bold_ == other.apply_bold_ &&
           apply_italic_ == other.apply_italic_ &&
           apply_underline_ == other.apply_underline_ &&
           apply_line_through_ == other.apply_line_through_ &&
           apply_subscript_ == other.apply_subscript_ &&
           apply_superscript_ == other.apply_superscript_ &&
           apply_font_color_ == other.apply_font_color_ &&
           apply_font_face_ == other.apply_font_face_ &&
           apply_font_size_ == other.apply_font_size_;
  }
  bool operator!=(const StyleChange& other) { return !(*this == other); }

 private:
  void ExtractTextStyles(Document*,
                         MutableCSSPropertyValueSet*,
                         bool is_monospace_font);

  String css_style_;
  bool apply_bold_;
  bool apply_italic_;
  bool apply_underline_;
  bool apply_line_through_;
  bool apply_subscript_;
  bool apply_superscript_;
  String apply_font_color_;
  String apply_font_face_;
  String apply_font_size_;
};

// FIXME: Remove these functions or make them non-global to discourage using
// CSSStyleDeclaration directly.
CSSValueID GetIdentifierValue(CSSStyleDeclaration*, CSSPropertyID);
CSSValueID GetIdentifierValue(CSSPropertyValueSet*, CSSPropertyID);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_EDITING_STYLE_H_
