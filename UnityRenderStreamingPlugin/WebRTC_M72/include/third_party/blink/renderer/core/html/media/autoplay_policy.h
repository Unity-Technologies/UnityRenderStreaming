// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_MEDIA_AUTOPLAY_POLICY_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_MEDIA_AUTOPLAY_POLICY_H_

#include "base/optional.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/bindings/exception_code.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class AutoplayUmaHelper;
class Document;
class ElementVisibilityObserver;
class HTMLMediaElement;

// AutoplayPolicy is the class for handles autoplay logics.
class AutoplayPolicy final : public GarbageCollected<AutoplayPolicy> {
 public:
  // Different autoplay policy types.
  enum class Type {
    kNoUserGestureRequired = 0,
    // A local user gesture on the element is required.
    kUserGestureRequired,
    // A local user gesture on the element is required when it is in a cross
    // origin iframe.
    kUserGestureRequiredForCrossOrigin,
    // The document needs to have received a user activation or received one
    // before navigating.
    kDocumentUserActivationRequired,
  };

  CORE_EXPORT static Type GetAutoplayPolicyForDocument(const Document&);

  // Return true if the given |document| is allowed to play.
  // This method may check parent frames if allow=autoplay (Feature Policy) was
  // used, in which case, the frame will be allowed to play if its parents are,
  // and so on.
  // Otherwise, frames are allowed to play if they have been activated or, for
  // the main frame, if it has a high MEI.
  CORE_EXPORT static bool IsDocumentAllowedToPlay(const Document&);

  // Returns true if the given |document| has high media engagement.
  static bool DocumentHasHighMediaEngagement(const Document&);

  // Returns true if the given |document| should force allow autoplay.
  static bool DocumentHasForceAllowFlag(const Document&);

  // Returns true if the given |document| has the user exception flag.
  static bool DocumentHasUserExceptionFlag(const Document&);

  // Returns true if the given |document| should autoplay muted videos.
  static bool DocumentShouldAutoplayMutedVideos(const Document&);

  // Returns true if the given |document| is capturing user media.
  static bool DocumentIsCapturingUserMedia(const Document&);

  explicit AutoplayPolicy(HTMLMediaElement*);

  void VideoWillBeDrawnToCanvas() const;

  // Called when the media element is moved to a new document.
  void DidMoveToNewDocument(Document& old_document);

  // Stop autoplaying the video element whenever its visible.
  // TODO(mlamouri): hide these methods from HTMLMediaElement.
  void StopAutoplayMutedWhenVisible();

  // Request autoplay by attribute. This method will check the autoplay
  // restrictions and record metrics. This method can only be called once per
  // time the readyState changes to HAVE_ENOUGH_DATA.
  bool RequestAutoplayByAttribute();

  // Request the playback via play() method. This method will check the autoplay
  // restrictions and record metrics. This method can only be called once
  // per call of play().
  base::Optional<DOMExceptionCode> RequestPlay();

  // Returns whether an umute action should pause an autoplaying element. The
  // method will check autoplay restrictions and record metrics. This method can
  // only be called once per call of setMuted().
  bool RequestAutoplayUnmute();

  // Indicates the media element is or will autoplay because of being
  // muted.
  CORE_EXPORT bool IsOrWillBeAutoplayingMuted() const;

  // Unlock user gesture if a user gesture can be utilized.
  void TryUnlockingUserGesture();

  // Return true if and only if a user gesture is requried for playback.  Even
  // if isLockedPendingUserGesture() return true, this might return false if
  // the requirement is currently overridden.  This does not check if a user
  // gesture is currently being processed.
  bool IsGestureNeededForPlayback() const;

  // Returns an error string to be used by the HTMLMediaElement when the play()
  // method fails because of autoplay restrictions.
  String GetPlayErrorMessage() const;

  // Returns whether the media element was initiated via autoplay.
  // In this context, autoplay means that it was initiated before any user
  // activation was received on the page and before a user initiated same-domain
  // navigation. In other words, with the unified autoplay policy applied, it
  // should only return `true` when MEI allowed autoplay.
  bool WasAutoplayInitiated() const;

  // Ensure that `autoplay_initiated_` has a value. It is set to `false` to
  // avoid false positives.
  void EnsureAutoplayInitiatedSet();

  virtual void Trace(blink::Visitor*);

 private:
  friend class AutoplayUmaHelper;
  friend class AutoplayUmaHelperTest;

  // Start autoplaying the video element whenever its visible.
  void StartAutoplayMutedWhenVisible();

  // Returns whether the media element is eligible to autoplay muted.
  bool IsEligibleForAutoplayMuted() const;

  bool ShouldAutoplay();

  // If the user gesture is required, then this will remove it.  Note that
  // one should not generally call this method directly; use the one on
  // m_helper and give it a reason.
  void UnlockUserGesture();

  // Return true if and only if a user gesture is required to unlock this
  // media element for unrestricted autoplay/script control.  Don't confuse
  // this with isGestureNeededForPlayback().  The latter is usually what one
  // should use, if checking to see if an action is allowed.
  bool IsLockedPendingUserGesture() const;

  bool IsLockedPendingUserGestureIfCrossOriginExperimentEnabled() const;

  bool IsGestureNeededForPlaybackIfCrossOriginExperimentEnabled() const;

  bool IsGestureNeededForPlaybackIfPendingUserGestureIsLocked() const;

  // Return true if and only if the settings allow autoplay of media on this
  // frame.
  bool IsAutoplayAllowedPerSettings() const;

  bool IsAutoplayingMutedInternal(bool muted) const;
  bool IsOrWillBeAutoplayingMutedInternal(bool muted) const;

  // Called when the video visibility changes while autoplaying muted, will
  // pause the video when invisible and resume the video when visible.
  void OnVisibilityChangedForAutoplay(bool is_visible);

  // Returns whether the current autoplay policy is
  // kDocumentUserActivationRequired. This is a helper method for readability.
  bool IsUsingDocumentUserActivationRequiredPolicy() const;

  // Sets `autoplay_initiated_` if it wasn't already set.
  void MaybeSetAutoplayInitiated();

  bool locked_pending_user_gesture_ : 1;
  bool locked_pending_user_gesture_if_cross_origin_experiment_enabled_ : 1;

  Member<HTMLMediaElement> element_;
  Member<ElementVisibilityObserver> autoplay_visibility_observer_;

  Member<AutoplayUmaHelper> autoplay_uma_helper_;

  base::Optional<bool> autoplay_initiated_;

  DISALLOW_COPY_AND_ASSIGN(AutoplayPolicy);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_MEDIA_AUTOPLAY_POLICY_H_
