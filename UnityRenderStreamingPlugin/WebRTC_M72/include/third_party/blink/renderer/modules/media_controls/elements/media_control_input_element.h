// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_INPUT_ELEMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_INPUT_ELEMENT_H_

#include "third_party/blink/renderer/core/html/forms/html_input_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_element_base.h"
#include "third_party/blink/renderer/modules/modules_export.h"

namespace blink {

class MediaControlsImpl;

// MediaControlElementBase implementation based on an <input> element. Used by
// buttons and sliders.
class MODULES_EXPORT MediaControlInputElement : public HTMLInputElement,
                                                public MediaControlElementBase {
  USING_GARBAGE_COLLECTED_MIXIN(MediaControlInputElement);

 public:
  static bool ShouldRecordDisplayStates(const HTMLMediaElement&);

  // Creates an overflow menu element with the given button as a child.
  HTMLElement* CreateOverflowElement(MediaControlInputElement*);

  // Implements MediaControlElementBase.
  void SetOverflowElementIsWanted(bool) final;
  void MaybeRecordDisplayed() final;

  void Trace(blink::Visitor*) override;

  MediaControlInputElement* OverflowElementForTests() const {
    return overflow_element_;
  }

  // Get the size of the element in pixels or the default if we cannot get the
  // size because the element has not been layed out yet.
  WebSize GetSizeOrDefault() const override;
  bool IsDisabled() const override;

 protected:
  MediaControlInputElement(MediaControlsImpl&, MediaControlElementType);

  // Returns a string that represents the button for metrics purposes. This
  // will be used as a suffix for histograms.
  virtual const char* GetNameForHistograms() const = 0;

  // Returns a string representation of the media control element.
  // Subclasses should override this method to return the string representation
  // of the overflow button.
  virtual WebLocalizedString::Name GetOverflowStringName() const;

  // Implements a default event handler to record interaction on click.
  void DefaultEventHandler(Event&) override;

  // Implements MediaControlElementBase.
  void UpdateShownState() override;

  // Updates the value of the Text string shown in the overflow menu.
  void UpdateOverflowString();

  // Record interaction if it wasn't recorded yet. It is used internally for
  // click events but also by some elements that have complex interaction logic.
  void MaybeRecordInteracted();

  // Returns whether this element is used for the overflow menu.
  bool IsOverflowElement() const;

  // Sets/removes a CSS class from this element based on |should_have_class|.
  void SetClass(const AtomicString& class_name, bool should_have_class);

  virtual void UpdateDisplayType();

 private:
  friend class MediaControlInputElementTest;

  bool IsMouseFocusable() const override;
  bool IsMediaControlElement() const final;

  // Returns a string representation of the media control element. Used for
  // the overflow menu.
  String GetOverflowMenuString() const;

  // Returns a subtitle for the overflow menu text, or a null String if there
  // should not be a subtitle.
  virtual String GetOverflowMenuSubtitleString() const;

  // Create/update subtitle text on the overflow element. If a null String is
  // given, the subtitle element is removed.
  void UpdateOverflowSubtitleElement(String text);

  // Remove the subtitle text from the overflow element.
  void RemoveOverflowSubtitleElement();

  // Used for histograms, do not reorder.
  enum class CTREvent {
    kDisplayed = 0,
    kInteracted,
    kCount,
  };

  // Records the CTR event.
  void RecordCTREvent(CTREvent);

  // The copy of this element used for the overflow menu in the media controls.
  // Setting this pointer is optional so it may be null.
  Member<MediaControlInputElement> overflow_element_;

  // Contains the overflow text and its subtitle (if exists).
  Member<HTMLDivElement> overflow_menu_container_;

  // The text representation of the button within the overflow menu.
  Member<HTMLSpanElement> overflow_menu_text_;

  // The subtitle of the text within the overflow menu.
  Member<HTMLSpanElement> overflow_menu_subtitle_;

  // Keeps track if the button was created for the purpose of the overflow menu.
  bool is_overflow_element_ = false;

  // Keeps track of whether the display/interaction have been recorded for the
  // CTR metrics.
  bool display_recorded_ = false;
  bool interaction_recorded_ = false;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_INPUT_ELEMENT_H_
