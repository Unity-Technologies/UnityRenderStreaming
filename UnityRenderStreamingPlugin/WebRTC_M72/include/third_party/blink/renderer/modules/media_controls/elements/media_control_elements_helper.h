// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_ELEMENTS_HELPER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_ELEMENTS_HELPER_H_

#include "third_party/blink/renderer/modules/media_controls/elements/media_control_element_type.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace WTF {
class AtomicString;
}  // namespace WTF

namespace blink {

class ContainerNode;
class Element;
class Event;
class HTMLDivElement;
class HTMLMediaElement;
class LayoutObject;
class Node;
struct WebSize;

// Helper class for media control elements. It contains methods, constants or
// concepts shared by more than one element.
class MediaControlElementsHelper final {
  STATIC_ONLY(MediaControlElementsHelper);

 public:
  static bool IsUserInteractionEvent(const Event&);

  // Sliders (the volume control and timeline) need to capture some additional
  // events used when dragging the thumb.
  static bool IsUserInteractionEventForSlider(const Event&, LayoutObject*);

  // Returns the MediaControlElementType associated with a given |Node|. The
  // |node| _must_ be a media control element.
  // Exported to be used by the accessibility module.
  MODULES_EXPORT static MediaControlElementType GetMediaControlElementType(
      const Node*);

  // Returns the media element associated with a given |node|.
  // Exported to be used by the accessibility module.
  MODULES_EXPORT static const HTMLMediaElement* ToParentMediaElement(
      const Node*);

  // Utility function for quickly creating div elements with a predefined shadow
  // ID.
  static HTMLDivElement* CreateDiv(const WTF::AtomicString& id,
                                   ContainerNode* parent);

  // Utility function for getting the size in pixels of an element. If the
  // element has not been layed out then returns the supplied default.
  static WebSize GetSizeOrDefault(const Element&, const WebSize&);

  // Utility function for quickly creating div elements with a predefined
  // element ID.
  static HTMLDivElement* CreateDivWithId(const WTF::AtomicString& id,
                                         ContainerNode* parent);

  // Utility function that notifies the media controls in which the element is
  // that it was focused by an accessibility tool.
  static void NotifyMediaControlAccessibleFocus(Element*);
  static void NotifyMediaControlAccessibleBlur(Element*);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_ELEMENTS_HELPER_H_
