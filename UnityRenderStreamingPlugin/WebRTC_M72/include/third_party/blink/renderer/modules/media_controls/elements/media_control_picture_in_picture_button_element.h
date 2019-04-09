// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_PICTURE_IN_PICTURE_BUTTON_ELEMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_PICTURE_IN_PICTURE_BUTTON_ELEMENT_H_

#include "third_party/blink/renderer/modules/media_controls/elements/media_control_input_element.h"

namespace blink {

class Event;
class MediaControlsImpl;

class MediaControlPictureInPictureButtonElement final
    : public MediaControlInputElement {
 public:
  explicit MediaControlPictureInPictureButtonElement(MediaControlsImpl&);

  // MediaControlInputElement:
  bool WillRespondToMouseClickEvents() override;
  void UpdateDisplayType() override;
  WebLocalizedString::Name GetOverflowStringName() const override;
  bool HasOverflowButton() const override;

  void OnMediaKeyboardEvent(Event* event) { DefaultEventHandler(*event); }

 protected:
  const char* GetNameForHistograms() const override;

 private:
  void DefaultEventHandler(Event&) override;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_PICTURE_IN_PICTURE_BUTTON_ELEMENT_H_
