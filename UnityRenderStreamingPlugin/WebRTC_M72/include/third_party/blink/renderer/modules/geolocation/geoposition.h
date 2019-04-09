/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_GEOLOCATION_GEOPOSITION_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_GEOLOCATION_GEOPOSITION_H_

#include "third_party/blink/renderer/core/dom/dom_time_stamp.h"
#include "third_party/blink/renderer/modules/event_modules.h"
#include "third_party/blink/renderer/modules/geolocation/coordinates.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/assertions.h"

namespace blink {

class Geoposition final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static Geoposition* Create(Coordinates* coordinates, DOMTimeStamp timestamp) {
    return MakeGarbageCollected<Geoposition>(coordinates, timestamp);
  }

  Geoposition(Coordinates* coordinates, DOMTimeStamp timestamp)
      : coordinates_(coordinates), timestamp_(timestamp) {
    DCHECK(coordinates_);
  }

  void Trace(blink::Visitor* visitor) override {
    visitor->Trace(coordinates_);
    ScriptWrappable::Trace(visitor);
  }

  DOMTimeStamp timestamp() const { return timestamp_; }
  Coordinates* coords() const { return coordinates_; }

 private:
  Member<Coordinates> coordinates_;
  DOMTimeStamp timestamp_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_GEOLOCATION_GEOPOSITION_H_
