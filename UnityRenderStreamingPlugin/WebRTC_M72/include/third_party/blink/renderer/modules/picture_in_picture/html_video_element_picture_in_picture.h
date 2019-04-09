// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_PICTURE_IN_PICTURE_HTML_VIDEO_ELEMENT_PICTURE_IN_PICTURE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_PICTURE_IN_PICTURE_HTML_VIDEO_ELEMENT_PICTURE_IN_PICTURE_H_

#include "third_party/blink/renderer/core/dom/qualified_name.h"
#include "third_party/blink/renderer/modules/event_target_modules.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class HTMLVideoElement;
class ScriptPromise;
class ScriptState;
class PictureInPictureControl;
struct PictureInPictureControlInfo;

class MODULES_EXPORT HTMLVideoElementPictureInPicture {
  STATIC_ONLY(HTMLVideoElementPictureInPicture);

 public:
  static ScriptPromise requestPictureInPicture(ScriptState*, HTMLVideoElement&);

  static void setPictureInPictureControls(
      HTMLVideoElement&,
      const HeapVector<Member<PictureInPictureControl>>&);

  static bool FastHasAttribute(const QualifiedName&, const HTMLVideoElement&);

  static void SetBooleanAttribute(const QualifiedName&,
                                  HTMLVideoElement&,
                                  bool);

  DEFINE_STATIC_ATTRIBUTE_EVENT_LISTENER(enterpictureinpicture,
                                         kEnterpictureinpicture);
  DEFINE_STATIC_ATTRIBUTE_EVENT_LISTENER(leavepictureinpicture,
                                         kLeavepictureinpicture);
  DEFINE_STATIC_ATTRIBUTE_EVENT_LISTENER(pictureinpicturecontrolclick,
                                         kPictureinpicturecontrolclick);

  static std::vector<PictureInPictureControlInfo>
  ToPictureInPictureControlInfoVector(
      const HeapVector<Member<PictureInPictureControl>>&);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_PICTURE_IN_PICTURE_HTML_VIDEO_ELEMENT_PICTURE_IN_PICTURE_H_
