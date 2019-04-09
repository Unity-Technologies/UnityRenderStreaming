/*
 * Copyright (C) 2013 Google Inc.  All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_TRACK_VTT_VTT_REGION_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_TRACK_VTT_VTT_REGION_H_

#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/geometry/double_point.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/timer.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class Document;
class ExceptionState;
class HTMLDivElement;
class VTTCueBox;
class VTTScanner;

class VTTRegion final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static VTTRegion* Create() { return new VTTRegion; }

  ~VTTRegion() override;

  const String& id() const { return id_; }
  void setId(const String&);

  double width() const { return width_; }
  void setWidth(double, ExceptionState&);

  unsigned lines() const { return lines_; }
  void setLines(unsigned);

  double regionAnchorX() const { return region_anchor_.X(); }
  void setRegionAnchorX(double, ExceptionState&);

  double regionAnchorY() const { return region_anchor_.Y(); }
  void setRegionAnchorY(double, ExceptionState&);

  double viewportAnchorX() const { return viewport_anchor_.X(); }
  void setViewportAnchorX(double, ExceptionState&);

  double viewportAnchorY() const { return viewport_anchor_.Y(); }
  void setViewportAnchorY(double, ExceptionState&);

  const AtomicString scroll() const;
  void setScroll(const AtomicString&);

  void SetRegionSettings(const String&);

  bool IsScrollingRegion() { return scroll_; }

  HTMLDivElement* GetDisplayTree(Document&);

  void AppendVTTCueBox(VTTCueBox*);
  void DisplayLastVTTCueBox();
  void WillRemoveVTTCueBox(VTTCueBox*);

  void Trace(blink::Visitor*) override;

 private:
  VTTRegion();

  void PrepareRegionDisplayTree();

  // The timer is needed to continue processing when cue scrolling ended.
  void StartTimer();
  void StopTimer();
  void ScrollTimerFired(TimerBase*);

  enum RegionSetting {
    kNone,
    kId,
    kWidth,
    kLines,
    kRegionAnchor,
    kViewportAnchor,
    kScroll
  };
  RegionSetting ScanSettingName(VTTScanner&);
  void ParseSettingValue(RegionSetting, VTTScanner&);

  static const AtomicString& TextTrackCueContainerScrollingClass();

  String id_;
  double width_;
  unsigned lines_;
  DoublePoint region_anchor_;
  DoublePoint viewport_anchor_;
  bool scroll_;

  // The cue container is the container that is scrolled up to obtain the
  // effect of scrolling cues when this is enabled for the regions.
  Member<HTMLDivElement> cue_container_;
  Member<HTMLDivElement> region_display_tree_;

  // Keep track of the current numeric value of the css "top" property.
  double current_top_;

  // The timer is used to display the next cue line after the current one has
  // been displayed. It's main use is for scrolling regions and it triggers as
  // soon as the animation for rolling out one line has finished, but
  // currently it is used also for non-scrolling regions to use a single
  // code path.
  TaskRunnerTimer<VTTRegion> scroll_timer_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_TRACK_VTT_VTT_REGION_H_
