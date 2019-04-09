// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_SOURCE_BUFFER_CLIENT_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_SOURCE_BUFFER_CLIENT_H_

#include "third_party/blink/public/platform/web_media_player.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/platform/web_vector.h"

namespace blink {

// Interface that the media engine uses to signal Blink's Media Source
// extensions module.
class WebSourceBufferClient {
 public:
  // Parser notification types used to monitor problematic usage.
  enum ParseWarning { kKeyframeTimeGreaterThanDependant, kMuxedSequenceMode };

  virtual ~WebSourceBufferClient() = default;

  // Complete media track info: track type, unique track id, kind, label,
  // language.
  struct MediaTrackInfo {
    WebMediaPlayer::TrackType track_type;
    WebMediaPlayer::TrackId id;
    WebString byte_stream_track_id;
    WebString kind;
    WebString label;
    WebString language;
  };

  // Notifies SourceBuffer that parsing of a new init segment has been completed
  // successfully. The input parameter is a collection of information about
  // media tracks found in the new init segment. The return value is true in
  // case of success.
  virtual bool InitializationSegmentReceived(
      const WebVector<MediaTrackInfo>& tracks) = 0;

  // Notifies SourceBuffer of parse warning.
  virtual void NotifyParseWarning(const ParseWarning) = 0;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_SOURCE_BUFFER_CLIENT_H_
