// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_TRACK_AUDIO_TRACK_LIST_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_TRACK_AUDIO_TRACK_LIST_H_

#include "third_party/blink/renderer/core/html/track/audio_track.h"
#include "third_party/blink/renderer/core/html/track/track_list_base.h"

namespace blink {

class CORE_EXPORT AudioTrackList final : public TrackListBase<AudioTrack> {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static AudioTrackList* Create(HTMLMediaElement&);

  ~AudioTrackList() override;

  bool HasEnabledTrack() const;

  // EventTarget
  const AtomicString& InterfaceName() const override;

  void Trace(blink::Visitor* visitor) override {
    TrackListBase<AudioTrack>::Trace(visitor);
  }

 private:
  explicit AudioTrackList(HTMLMediaElement&);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_TRACK_AUDIO_TRACK_LIST_H_
