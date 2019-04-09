/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010 Apple Inc. All rights
 * reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_FORMS_HTML_FORM_CONTROL_ELEMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_FORMS_HTML_FORM_CONTROL_ELEMENT_H_

#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/web/web_autofill_state.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/html/forms/form_associated.h"
#include "third_party/blink/renderer/core/html/forms/labelable_element.h"
#include "third_party/blink/renderer/core/html/forms/listed_element.h"

namespace blink {

class HTMLFormElement;
class ValidationMessageClient;

enum CheckValidityEventBehavior {
  kCheckValidityDispatchNoEvent,
  kCheckValidityDispatchInvalidEvent
};

// HTMLFormControlElement is the default implementation of
// ListedElement, and listed element implementations should use
// HTMLFormControlElement unless there is a special reason.
class CORE_EXPORT HTMLFormControlElement : public LabelableElement,
                                           public ListedElement,
                                           public FormAssociated {
  USING_GARBAGE_COLLECTED_MIXIN(HTMLFormControlElement);

 public:
  ~HTMLFormControlElement() override;
  void Trace(blink::Visitor*) override;

  String formAction() const;
  void setFormAction(const AtomicString&);
  String formEnctype() const;
  void setFormEnctype(const AtomicString&);
  String formMethod() const;
  void setFormMethod(const AtomicString&);
  bool FormNoValidate() const;

  void AncestorDisabledStateWasChanged();

  void Reset();

  void DispatchChangeEvent();

  HTMLFormElement* formOwner() const final;

  bool IsDisabledFormControl() const override;

  bool MatchesEnabledPseudoClass() const override;

  bool IsEnumeratable() const override { return false; }

  bool IsRequired() const;

  const AtomicString& type() const { return FormControlType(); }

  virtual const AtomicString& FormControlType() const = 0;

  virtual bool CanTriggerImplicitSubmission() const { return false; }

  virtual bool IsSubmittableElement() { return true; }

  virtual String ResultForDialogSubmit();

  // Return true if this control type can be a submit button.  This doesn't
  // check |disabled|, and this doesn't check if this is the first submit
  // button.
  virtual bool CanBeSuccessfulSubmitButton() const { return false; }
  // Return true if this control can submit a form.
  // i.e. canBeSuccessfulSubmitButton() && !isDisabledFormControl().
  bool IsSuccessfulSubmitButton() const;
  virtual bool IsActivatedSubmit() const { return false; }
  virtual void SetActivatedSubmit(bool) {}

  bool willValidate() const override;

  void UpdateVisibleValidationMessage();
  void HideVisibleValidationMessage();
  bool checkValidity(
      HeapVector<Member<HTMLFormControlElement>>* unhandled_invalid_controls =
          nullptr,
      CheckValidityEventBehavior = kCheckValidityDispatchInvalidEvent);
  bool reportValidity();
  // This must be called only after the caller check the element is focusable.
  void ShowValidationMessage();
  bool IsValidationMessageVisible() const;
  // This must be called when a validation constraint or control value is
  // changed.
  void SetNeedsValidityCheck();
  void setCustomValidity(const String&) final;
  void FindCustomValidationMessageTextDirection(const String& message,
                                                TextDirection& message_dir,
                                                String& sub_message,
                                                TextDirection& sub_message_dir);

  bool IsReadOnly() const;
  bool IsDisabledOrReadOnly() const;

  bool IsAutofocusable() const;

  bool MayTriggerVirtualKeyboard() const override;

  WebAutofillState GetAutofillState() const { return autofill_state_; }
  bool IsAutofilled() const {
    return autofill_state_ != WebAutofillState::kNotFilled;
  }
  void SetAutofillState(WebAutofillState = WebAutofillState::kAutofilled);

  // The autofill section to which this element belongs (e.g. billing address,
  // shipping address, .. .)
  WebString AutofillSection() const { return autofill_section_; }
  void SetAutofillSection(const WebString&);

  const AtomicString& autocapitalize() const final;

  static const HTMLFormControlElement* EnclosingFormControlElement(const Node*);

  String NameForAutofill() const;

  void CloneNonAttributePropertiesFrom(const Element&,
                                       CloneChildrenFlag) override;

  FormAssociated* ToFormAssociatedOrNull() override { return this; };
  void AssociateWith(HTMLFormElement*) override;

  bool BlocksFormSubmission() const { return blocks_form_submission_; }
  void SetBlocksFormSubmission(bool value) { blocks_form_submission_ = value; }

  unsigned UniqueRendererFormControlId() const {
    return unique_renderer_form_control_id_;
  }

 protected:
  HTMLFormControlElement(const QualifiedName& tag_name, Document&);

  void AttributeChanged(const AttributeModificationParams&) override;
  void ParseAttribute(const AttributeModificationParams&) override;
  virtual void RequiredAttributeChanged();
  virtual void DisabledAttributeChanged();
  void AttachLayoutTree(AttachContext&) override;
  InsertionNotificationRequest InsertedInto(ContainerNode&) override;
  void RemovedFrom(ContainerNode&) override;
  void WillChangeForm() override;
  void DidChangeForm() override;
  void DidMoveToNewDocument(Document& old_document) override;

  bool SupportsFocus() const override;
  bool IsKeyboardFocusable() const override;
  bool ShouldHaveFocusAppearance() const final;
  void DispatchBlurEvent(Element* new_focused_element,
                         WebFocusType,
                         InputDeviceCapabilities* source_capabilities) override;

  void DidRecalcStyle(StyleRecalcChange) override;

  // This must be called any time the result of willValidate() has changed.
  void SetNeedsWillValidateCheck();
  virtual bool RecalcWillValidate() const;

  virtual void ResetImpl() {}
  virtual bool SupportsAutofocus() const;

 private:
  bool IsFormControlElement() const final { return true; }
  bool AlwaysCreateUserAgentShadowRoot() const override { return true; }

  int tabIndex() const override;

  bool IsValidElement() override;
  bool MatchesValidityPseudoClasses() const override;
  void UpdateAncestorDisabledState() const;

  ValidationMessageClient* GetValidationMessageClient() const;

  // Requests validity recalc for the form owner, if one exists.
  void FormOwnerSetNeedsValidityCheck();
  // Requests validity recalc for all ancestor fieldsets, if exist.
  void FieldSetAncestorsSetNeedsValidityCheck(Node*);

  unsigned unique_renderer_form_control_id_;

  WebString autofill_section_;
  enum WebAutofillState autofill_state_;

  enum AncestorDisabledState {
    kAncestorDisabledStateUnknown,
    kAncestorDisabledStateEnabled,
    kAncestorDisabledStateDisabled
  };

  mutable AncestorDisabledState ancestor_disabled_state_;
  enum DataListAncestorState { kUnknown, kInsideDataList, kNotInsideDataList };
  mutable enum DataListAncestorState data_list_ancestor_state_;
  mutable bool may_have_field_set_ancestor_ : 1;

  bool has_validation_message_ : 1;
  // The initial value of will_validate_ depends on the derived class. We can't
  // initialize it with a virtual function in the constructor. will_validate_
  // is not deterministic as long as will_validate_initialized_ is false.
  mutable bool will_validate_initialized_ : 1;
  mutable bool will_validate_ : 1;

  // Cache of valid().
  bool is_valid_ : 1;
  bool validity_is_dirty_ : 1;

  bool blocks_form_submission_ : 1;
};

inline bool IsHTMLFormControlElement(const Element& element) {
  return element.IsFormControlElement();
}

DEFINE_HTMLELEMENT_TYPE_CASTS_WITH_FUNCTION(HTMLFormControlElement);
DEFINE_TYPE_CASTS(HTMLFormControlElement,
                  ListedElement,
                  control,
                  control->IsFormControlElement(),
                  control.IsFormControlElement());

}  // namespace blink

#endif
