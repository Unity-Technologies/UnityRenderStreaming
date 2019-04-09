// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FULLSCREEN_ELEMENT_FULLSCREEN_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FULLSCREEN_ELEMENT_FULLSCREEN_H_

#include "third_party/blink/renderer/bindings/core/v8/script_promise.h"
#include "third_party/blink/renderer/core/dom/events/event_target.h"
#include "third_party/blink/renderer/core/fullscreen/fullscreen_options.h"
#include "third_party/blink/renderer/platform/bindings/script_state.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace blink {

class Element;

class ElementFullscreen {
  STATIC_ONLY(ElementFullscreen);

 public:
  static ScriptPromise requestFullscreen(ScriptState*,
                                         Element&,
                                         const FullscreenOptions*);

  DEFINE_STATIC_ATTRIBUTE_EVENT_LISTENER(fullscreenchange, kFullscreenchange);
  DEFINE_STATIC_ATTRIBUTE_EVENT_LISTENER(fullscreenerror, kFullscreenerror);

  static void webkitRequestFullscreen(Element&);
  static void webkitRequestFullscreen(Element&, const FullscreenOptions*);

  DEFINE_STATIC_ATTRIBUTE_EVENT_LISTENER(webkitfullscreenchange,
                                         kWebkitfullscreenchange);
  DEFINE_STATIC_ATTRIBUTE_EVENT_LISTENER(webkitfullscreenerror,
                                         kWebkitfullscreenerror);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FULLSCREEN_ELEMENT_FULLSCREEN_H_
