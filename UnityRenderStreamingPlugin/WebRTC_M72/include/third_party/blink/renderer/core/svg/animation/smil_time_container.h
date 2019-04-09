/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_ANIMATION_SMIL_TIME_CONTAINER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_ANIMATION_SMIL_TIME_CONTAINER_H_

#include "third_party/blink/renderer/core/dom/qualified_name.h"
#include "third_party/blink/renderer/platform/graphics/image_animation_policy.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/timer.h"
#include "third_party/blink/renderer/platform/wtf/hash_map.h"
#include "third_party/blink/renderer/platform/wtf/hash_set.h"
#include "third_party/blink/renderer/platform/wtf/text/string_hash.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class Document;
class SMILTime;
class SVGElement;
class SVGSMILElement;
class SVGSVGElement;

class SMILTimeContainer : public GarbageCollectedFinalized<SMILTimeContainer> {
 public:
  static SMILTimeContainer* Create(SVGSVGElement& owner) {
    return new SMILTimeContainer(owner);
  }
  ~SMILTimeContainer();

  void Schedule(SVGSMILElement*, SVGElement*, const QualifiedName&);
  void Unschedule(SVGSMILElement*, SVGElement*, const QualifiedName&);
  void NotifyIntervalsChanged();

  double Elapsed() const;

  bool IsPaused() const;
  bool IsStarted() const;

  void Start();
  void Pause();
  void Unpause();
  void SetElapsed(double);

  void ServiceAnimations();
  bool HasAnimations() const;

  void SetDocumentOrderIndexesDirty() { document_order_indexes_dirty_ = true; }

  // Advance the animation timeline a single frame.
  void AdvanceFrameForTesting();

  void Trace(blink::Visitor*);

 private:
  explicit SMILTimeContainer(SVGSVGElement& owner);

  enum FrameSchedulingState {
    // No frame scheduled.
    kIdle,
    // Scheduled a wakeup to update the animation values.
    kSynchronizeAnimations,
    // Scheduled a wakeup to trigger an animation frame.
    kFutureAnimationFrame,
    // Scheduled a animation frame for continuous update.
    kAnimationFrame
  };

  enum AnimationPolicyOnceAction {
    // Restart OnceTimer if the timeline is not paused.
    kRestartOnceTimerIfNotPaused,
    // Restart OnceTimer.
    kRestartOnceTimer,
    // Cancel OnceTimer.
    kCancelOnceTimer
  };

  bool IsTimelineRunning() const;
  void SynchronizeToDocumentTimeline();
  void ScheduleAnimationFrame(double delay_time);
  void CancelAnimationFrame();
  void WakeupTimerFired(TimerBase*);
  void ScheduleAnimationPolicyTimer();
  void CancelAnimationPolicyTimer();
  void AnimationPolicyTimerFired(TimerBase*);
  ImageAnimationPolicy AnimationPolicy() const;
  bool HandleAnimationPolicy(AnimationPolicyOnceAction);
  bool CanScheduleFrame(SMILTime earliest_fire_time) const;
  void UpdateAnimationsAndScheduleFrameIfNeeded(double elapsed,
                                                bool seek_to_time = false);
  SMILTime UpdateAnimations(double elapsed, bool seek_to_time);
  void ServiceOnNextFrame();
  void ScheduleWakeUp(double delay_time, FrameSchedulingState);
  bool HasPendingSynchronization() const;

  void UpdateDocumentOrderIndexes();

  SVGSVGElement& OwnerSVGElement() const;
  Document& GetDocument() const;

  // The latest "restart" time for the time container's timeline. If the
  // timeline has not been manipulated (seeked, paused) this will be zero.
  double presentation_time_;
  // The time on the document timeline corresponding to |m_presentationTime|.
  double reference_time_;

  FrameSchedulingState frame_scheduling_state_;
  bool started_;  // The timeline has been started.
  bool paused_;   // The timeline is paused.

  bool document_order_indexes_dirty_;

  TaskRunnerTimer<SMILTimeContainer> wakeup_timer_;
  TaskRunnerTimer<SMILTimeContainer> animation_policy_once_timer_;

  using AnimationsLinkedHashSet = HeapLinkedHashSet<WeakMember<SVGSMILElement>>;
  using AttributeAnimationsMap =
      HeapHashMap<QualifiedName, Member<AnimationsLinkedHashSet>>;
  using GroupedAnimationsMap =
      HeapHashMap<WeakMember<SVGElement>, AttributeAnimationsMap>;
  GroupedAnimationsMap scheduled_animations_;

  Member<SVGSVGElement> owner_svg_element_;

#if DCHECK_IS_ON()
  bool prevent_scheduled_animations_changes_ = false;
#endif
};
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_ANIMATION_SMIL_TIME_CONTAINER_H_
