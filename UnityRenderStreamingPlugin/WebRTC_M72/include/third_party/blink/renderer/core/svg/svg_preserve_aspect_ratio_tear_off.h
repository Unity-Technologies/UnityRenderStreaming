/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_PRESERVE_ASPECT_RATIO_TEAR_OFF_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_PRESERVE_ASPECT_RATIO_TEAR_OFF_H_

#include "third_party/blink/renderer/core/svg/properties/svg_property_tear_off.h"
#include "third_party/blink/renderer/core/svg/svg_preserve_aspect_ratio.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class SVGPreserveAspectRatioTearOff final
    : public SVGPropertyTearOff<SVGPreserveAspectRatio> {
  DEFINE_WRAPPERTYPEINFO();

 public:
  enum {
    kSvgPreserveaspectratioUnknown =
        SVGPreserveAspectRatio::kSvgPreserveaspectratioUnknown,
    kSvgPreserveaspectratioNone =
        SVGPreserveAspectRatio::kSvgPreserveaspectratioNone,
    kSvgPreserveaspectratioXminymin =
        SVGPreserveAspectRatio::kSvgPreserveaspectratioXminymin,
    kSvgPreserveaspectratioXmidymin =
        SVGPreserveAspectRatio::kSvgPreserveaspectratioXmidymin,
    kSvgPreserveaspectratioXmaxymin =
        SVGPreserveAspectRatio::kSvgPreserveaspectratioXmaxymin,
    kSvgPreserveaspectratioXminymid =
        SVGPreserveAspectRatio::kSvgPreserveaspectratioXminymid,
    kSvgPreserveaspectratioXmidymid =
        SVGPreserveAspectRatio::kSvgPreserveaspectratioXmidymid,
    kSvgPreserveaspectratioXmaxymid =
        SVGPreserveAspectRatio::kSvgPreserveaspectratioXmaxymid,
    kSvgPreserveaspectratioXminymax =
        SVGPreserveAspectRatio::kSvgPreserveaspectratioXminymax,
    kSvgPreserveaspectratioXmidymax =
        SVGPreserveAspectRatio::kSvgPreserveaspectratioXmidymax,
    kSvgPreserveaspectratioXmaxymax =
        SVGPreserveAspectRatio::kSvgPreserveaspectratioXmaxymax
  };

  enum {
    kSvgMeetorsliceUnknown = SVGPreserveAspectRatio::kSvgMeetorsliceUnknown,
    kSvgMeetorsliceMeet = SVGPreserveAspectRatio::kSvgMeetorsliceMeet,
    kSvgMeetorsliceSlice = SVGPreserveAspectRatio::kSvgMeetorsliceSlice
  };

  static SVGPreserveAspectRatioTearOff* Create(
      SVGPreserveAspectRatio* target,
      SVGAnimatedPropertyBase* binding,
      PropertyIsAnimValType property_is_anim_val) {
    return MakeGarbageCollected<SVGPreserveAspectRatioTearOff>(
        target, binding, property_is_anim_val);
  }

  SVGPreserveAspectRatioTearOff(SVGPreserveAspectRatio*,
                                SVGAnimatedPropertyBase* binding,
                                PropertyIsAnimValType);

  void setAlign(unsigned short, ExceptionState&);
  unsigned short align() { return Target()->Align(); }
  void setMeetOrSlice(unsigned short, ExceptionState&);
  unsigned short meetOrSlice() { return Target()->MeetOrSlice(); }
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_PRESERVE_ASPECT_RATIO_TEAR_OFF_H_
