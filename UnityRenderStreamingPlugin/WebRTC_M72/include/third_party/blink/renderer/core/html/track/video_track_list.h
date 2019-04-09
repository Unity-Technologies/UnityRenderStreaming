// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_TRACK_VIDEO_TRACK_LIST_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_TRACK_VIDEO_TRACK_LIST_H_

#include "third_party/blink/renderer/core/html/track/track_list_base.h"
#include "third_party/blink/renderer/core/html/track/video_track.h"

namespace blink {

class CORE_EXPORT VideoTrackList final : public TrackListBase<VideoTrack> {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static VideoTrackList* Create(HTMLMediaElement&);

  ~VideoTrackList() override;

  int selectedIndex() const;

  // EventTarget
  const AtomicString& InterfaceName() const override;

  void TrackSelected(WebMediaPlayer::TrackId selected_track_id);

  void Trace(blink::Visitor* visitor) override {
    TrackListBase<VideoTrack>::Trace(visitor);
  }

 private:
  explicit VideoTrackList(HTMLMediaElement&);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_TRACK_VIDEO_TRACK_LIST_H_
