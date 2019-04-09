// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_TIME_DISPLAY_ELEMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_TIME_DISPLAY_ELEMENT_H_

#include "third_party/blink/renderer/modules/media_controls/elements/media_control_div_element.h"
#include "third_party/blink/renderer/modules/modules_export.h"

namespace blink {

class MediaControlsImpl;

class MediaControlTimeDisplayElement : public MediaControlDivElement {
 public:
  // Exported to be used in unit tests.
  MODULES_EXPORT void SetCurrentValue(double);
  // Exported to be used by modules/accessibility.
  MODULES_EXPORT double CurrentValue() const;

 protected:
  MediaControlTimeDisplayElement(MediaControlsImpl&, MediaControlElementType);

  virtual String FormatTime() const;

 private:
  double current_value_ = 0;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_TIME_DISPLAY_ELEMENT_H_
