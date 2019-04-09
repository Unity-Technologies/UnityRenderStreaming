// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_ELEMENT_BASE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_ELEMENT_BASE_H_

#include "third_party/blink/public/platform/web_localized_string.h"
#include "third_party/blink/renderer/core/dom/element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_element_type.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/heap/visitor.h"

namespace blink {

class Element;
class HTMLElement;
class HTMLMediaElement;
class MediaControlsImpl;
struct WebSize;

// MediaControlElementBase is the base class for all the media control elements.
// It is sub-classed by MediaControlInputElement and MediaControlDivElement
// which are then used by the final implementations.
class MODULES_EXPORT MediaControlElementBase : public GarbageCollectedMixin {
 public:
  // These hold the state about whether this control should be shown if
  // space permits.  These will also show / hide as needed.
  virtual void SetIsWanted(bool);
  bool IsWanted() const;

  // Tell us whether we fit or not.  This will hide / show the control as
  // needed, also.
  void SetDoesFit(bool);
  bool DoesFit() const;

  // Similar to SetIsWanted() for the overflow element associated to the current
  // element. Will be a no-op if the element does not have an associated
  // overflow element.
  virtual void SetOverflowElementIsWanted(bool) = 0;

  // Called when recording the display state of the media control element should
  // happen. It will be a no-op if the element isn't displayed in the controls.
  virtual void MaybeRecordDisplayed() = 0;

  // Returns the display type of the element that is set at creation.
  MediaControlElementType DisplayType() const;

  // By default, media controls elements are not added to the overflow menu.
  // Controls that can be added to the overflow menu should override this
  // function and return true.
  virtual bool HasOverflowButton() const;

  // Get the size of the element in pixels or the default if we cannot get the
  // size because the element has not been layed out yet.
  virtual WebSize GetSizeOrDefault() const = 0;

  // Whether the element has been disabled via the HTML disabled attribute.
  virtual bool IsDisabled() const = 0;

  void Trace(blink::Visitor*) override;

 protected:
  MediaControlElementBase(MediaControlsImpl&,
                          MediaControlElementType,
                          HTMLElement*);

  // Hide or show based on our fits / wanted state.  We want to show
  // if and only if we're wanted and we fit.
  virtual void UpdateShownState();

  MediaControlsImpl& GetMediaControls() const;

  HTMLMediaElement& MediaElement() const;

  void SetDisplayType(MediaControlElementType);

 private:
  Member<MediaControlsImpl> media_controls_;
  MediaControlElementType display_type_;
  Member<HTMLElement> element_;
  bool is_wanted_ : 1;
  bool does_fit_ : 1;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_ELEMENT_BASE_H_
