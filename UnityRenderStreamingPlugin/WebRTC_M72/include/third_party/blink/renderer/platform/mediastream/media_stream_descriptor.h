/*
 * Copyright (C) 2011 Ericsson AB. All rights reserved.
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Ericsson nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIASTREAM_MEDIA_STREAM_DESCRIPTOR_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIASTREAM_MEDIA_STREAM_DESCRIPTOR_H_

#include <memory>
#include "third_party/blink/renderer/platform/mediastream/media_stream_component.h"
#include "third_party/blink/renderer/platform/mediastream/media_stream_source.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class WebMediaStreamObserver;

class PLATFORM_EXPORT MediaStreamDescriptorClient
    : public GarbageCollectedMixin {
 public:
  virtual ~MediaStreamDescriptorClient() = default;

  virtual void StreamEnded() = 0;
  virtual void AddTrackByComponentAndFireEvents(MediaStreamComponent*) = 0;
  virtual void RemoveTrackByComponentAndFireEvents(MediaStreamComponent*) = 0;
  void Trace(blink::Visitor* visitor) override {}
};

class PLATFORM_EXPORT MediaStreamDescriptor final
    : public GarbageCollectedFinalized<MediaStreamDescriptor> {
 private:
  static int GenerateUniqueId();

 public:
  // Only used for AudioDestinationNode.
  static MediaStreamDescriptor* Create(
      const MediaStreamSourceVector& audio_sources,
      const MediaStreamSourceVector& video_sources);

  static MediaStreamDescriptor* Create(
      const MediaStreamComponentVector& audio_components,
      const MediaStreamComponentVector& video_components);

  static MediaStreamDescriptor* Create(
      const String& id,
      const MediaStreamComponentVector& audio_components,
      const MediaStreamComponentVector& video_components);

  MediaStreamDescriptor(const String& id,
                        const MediaStreamSourceVector& audio_sources,
                        const MediaStreamSourceVector& video_sources);
  MediaStreamDescriptor(const String& id,
                        const MediaStreamComponentVector& audio_components,
                        const MediaStreamComponentVector& video_components);

  MediaStreamDescriptorClient* Client() const { return client_; }
  void SetClient(MediaStreamDescriptorClient* client) { client_ = client; }

  // This is the same as the id of the |MediaStream|. It is unique in most
  // contexts but collisions can occur e.g. if streams are created by different
  // |RTCPeerConnection|s or a remote stream ID is signaled to be added, removed
  // and then re-added resulting in a new stream object the second time around.
  String Id() const { return id_; }
  // Uniquely identifies this descriptor.
  int UniqueId() const { return unique_id_; }

  unsigned NumberOfAudioComponents() const { return audio_components_.size(); }
  MediaStreamComponent* AudioComponent(unsigned index) const {
    return audio_components_[index].Get();
  }

  unsigned NumberOfVideoComponents() const { return video_components_.size(); }
  MediaStreamComponent* VideoComponent(unsigned index) const {
    return video_components_[index].Get();
  }

  void AddComponent(MediaStreamComponent*);
  void RemoveComponent(MediaStreamComponent*);

  void AddRemoteTrack(MediaStreamComponent*);
  void RemoveRemoteTrack(MediaStreamComponent*);

  bool Active() const { return active_; }
  void SetActive(bool active);

  void AddObserver(WebMediaStreamObserver*);
  void RemoveObserver(WebMediaStreamObserver*);

  // |m_extraData| may hold pointers to GC objects, and it may touch them in
  // destruction.  So this class is eagerly finalized to finalize |m_extraData|
  // promptly.
  EAGERLY_FINALIZE();
  void Trace(blink::Visitor*);

 private:
  Member<MediaStreamDescriptorClient> client_;
  String id_;
  int unique_id_;
  HeapVector<Member<MediaStreamComponent>> audio_components_;
  HeapVector<Member<MediaStreamComponent>> video_components_;
  Vector<WebMediaStreamObserver*> observers_;
  bool active_;
};

typedef HeapVector<Member<MediaStreamDescriptor>> MediaStreamDescriptorVector;

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_MEDIASTREAM_MEDIA_STREAM_DESCRIPTOR_H_
