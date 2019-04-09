// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EVENTS_PICTURE_IN_PICTURE_CONTROL_EVENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EVENTS_PICTURE_IN_PICTURE_CONTROL_EVENT_H_

#include "third_party/blink/renderer/core/dom/events/event.h"
#include "third_party/blink/renderer/core/events/picture_in_picture_control_event_init.h"

namespace blink {

// A PictureInPictureControlEvent is a subclass of Event with an additional
// string. This string is used to convey to the browser the name of a custom
// control on a Picture in Picture window.
class CORE_EXPORT PictureInPictureControlEvent final : public Event {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static PictureInPictureControlEvent* Create(const AtomicString&, String);
  static PictureInPictureControlEvent* Create(
      const AtomicString&,
      const PictureInPictureControlEventInit*);

  String id() const;
  void setId(String id);

 private:
  PictureInPictureControlEvent(AtomicString const&, String);
  PictureInPictureControlEvent(AtomicString const&,
                               const PictureInPictureControlEventInit*);

  // id_ holds the id of a PictureIncPictureControlEvent, which is used to
  // convey which custom control fired the event by being clicked.
  String id_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_EVENTS_PICTURE_IN_PICTURE_CONTROL_EVENT_H_
