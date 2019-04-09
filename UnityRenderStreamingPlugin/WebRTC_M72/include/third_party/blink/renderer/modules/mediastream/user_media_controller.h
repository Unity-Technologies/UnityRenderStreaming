/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIASTREAM_USER_MEDIA_CONTROLLER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIASTREAM_USER_MEDIA_CONTROLLER_H_

#include <memory>

#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/modules/mediastream/user_media_client.h"

namespace blink {

class ApplyConstraintsRequest;
class MediaStreamComponent;
class UserMediaRequest;

class UserMediaController final
    : public GarbageCollectedFinalized<UserMediaController>,
      public Supplement<LocalFrame> {
  USING_GARBAGE_COLLECTED_MIXIN(UserMediaController);

 public:
  static const char kSupplementName[];

  UserMediaController(LocalFrame&, std::unique_ptr<UserMediaClient>);
  void Trace(blink::Visitor*) override;

  UserMediaClient* Client() const { return client_.get(); }

  void RequestUserMedia(UserMediaRequest*);
  void CancelUserMediaRequest(UserMediaRequest*);
  void ApplyConstraints(ApplyConstraintsRequest*);
  void StopTrack(MediaStreamComponent*);
  bool HasRequestedUserMedia();

  static UserMediaController* From(LocalFrame* frame) {
    return Supplement<LocalFrame>::From<UserMediaController>(frame);
  }

 private:
  std::unique_ptr<UserMediaClient> client_;
  bool has_requested_user_media_ = false;
};

inline void UserMediaController::RequestUserMedia(UserMediaRequest* request) {
  client_->RequestUserMedia(request);
  has_requested_user_media_ = true;
}

inline void UserMediaController::CancelUserMediaRequest(
    UserMediaRequest* request) {
  client_->CancelUserMediaRequest(request);
}

inline void UserMediaController::ApplyConstraints(
    ApplyConstraintsRequest* request) {
  client_->ApplyConstraints(request);
}

inline void UserMediaController::StopTrack(MediaStreamComponent* track) {
  client_->StopTrack(track);
}

inline bool UserMediaController::HasRequestedUserMedia() {
  return has_requested_user_media_;
}

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIASTREAM_USER_MEDIA_CONTROLLER_H_
