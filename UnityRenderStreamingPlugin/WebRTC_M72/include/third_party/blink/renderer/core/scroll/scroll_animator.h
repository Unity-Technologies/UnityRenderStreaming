/*
 * Copyright (c) 2011, Google Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SCROLL_SCROLL_ANIMATOR_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SCROLL_SCROLL_ANIMATOR_H_

#include <memory>
#include "third_party/blink/renderer/core/scroll/scroll_animator_base.h"
#include "third_party/blink/renderer/platform/animation/compositor_animation_client.h"
#include "third_party/blink/renderer/platform/animation/compositor_animation_delegate.h"
#include "third_party/blink/renderer/platform/animation/compositor_scroll_offset_animation_curve.h"
#include "third_party/blink/renderer/platform/timer.h"

namespace blink {

class CompositorAnimationTimeline;

// ScrollAnimator is the Blink-side implementation of user-input scroll offset
// animations ("smooth scrolling") on all platforms except for Mac.
//
// See http://bit.ly/smoothscrolling for general info about user-input smooth
// scrolling.  For the Mac implementation, see ScrollAnimatorMac.  For
// programmatic (CSSOM) smooth scrolls, see ProgrammaticScrollAnimator.
//
// When Blink receives an input event that should start or update a scroll
// animation, it calls ScrollAnimator::UserScroll.  This will construct an
// animation curve object which can report the desired scroll offset as a
// function of elapsed time.  See cc/animation/scroll_offset_animation_curve.h
// for more info about the animation curve logic (including velocity-matched
// target updating).
//
// Having established an animation curve, the logic for servicing the animation
// is highly dependent on compositing.  There are four scenarios to consider:
//
// (1) Scroll animation running on the compositor, scheduled by the compositor
//     (LayerTreeHostImpl::ScrollAnimated) in response to a scroll wheel input
//     event handled by the compositor thread.  Blink doesn't know about these.
//
// (2) Scroll animation running on the compositor, scheduled by Blink.  For
//     example, a keyboard scroll of a composited scroller.
//
// (3) Scroll animation of a composited scroller, running on the main thread due
//     to main-thread scrolling reasons (for example, non-composited fixed-
//     position elements that need to be repainted on scroll).
//
// (4) Scroll animation of a non-composited scroller, running on the main
//     thread.
//
// In scenarios (1) and (2) the animation is created as a cc::Animation with
// TargetProperty::SCROLL_OFFSET and added to a cc::Animation that is
// serviced on the compositor thread (in cc::AnimationHost::TickAnimations).
// This lets the animation play smoothly even if the main thread is janked.
//
// In scenarios (3) and (4), we schedule the animation ticks on the main thread
// using ScrollableArea::ScheduleAnimation, and update the scroll offset during
// ScrollAnimator::TickAnimation.
//
// There is a special main-thread scrolling reason kHandlingScrollFromMainThread
// set in scenarios (2) and (3) for the duration of the scroll, to prevent
// interference from events that would otherwise trigger scenario (1).
//
// There is a complicated handoff from (1) to (3) in the event that a main-
// thread scrolling reason is added in the middle of an animation.  This is
// handled by TakeOverCompositorAnimation, which aborts the animation in cc and
// sends an AnimationEvent::TAKEOVER back to the main thread containing a copy
// of the curve.  That calls back into NotifyAnimationTakeover which starts a
// new animation on main to play the "remainder" of the curve.
//
// The logic for Blink-side scheduling of compositor-serviced scroll offset
// animations is shared with ProgrammaticScrollAnimator, and lives mostly in the
// common base class ScrollAnimatorCompositorCoordinator.

class CORE_EXPORT ScrollAnimator : public ScrollAnimatorBase {
 public:
  explicit ScrollAnimator(ScrollableArea*,
                          WTF::TimeFunction = WTF::CurrentTimeTicksInSeconds);
  ~ScrollAnimator() override;

  bool HasRunningAnimation() const override;
  ScrollOffset ComputeDeltaToConsume(const ScrollOffset& delta) const override;

  ScrollResult UserScroll(ScrollGranularity,
                          const ScrollOffset& delta) override;
  void ScrollToOffsetWithoutAnimation(const ScrollOffset&) override;
  ScrollOffset DesiredTargetOffset() const override;

  // ScrollAnimatorCompositorCoordinator implementation.
  void TickAnimation(double monotonic_time) override;
  void CancelAnimation() override;
  void AdjustAnimationAndSetScrollOffset(const ScrollOffset&,
                                         ScrollType) override;
  void TakeOverCompositorAnimation() override;
  void ResetAnimationState() override;
  void UpdateCompositorAnimations() override;
  void NotifyCompositorAnimationFinished(int group_id) override;
  void NotifyCompositorAnimationAborted(int group_id) override;
  void LayerForCompositedScrollingDidChange(
      CompositorAnimationTimeline*) override;

  void Trace(blink::Visitor*) override;

 protected:
  // Returns whether or not the animation was sent to the compositor.
  virtual bool SendAnimationToCompositor();

  void NotifyAnimationTakeover(double monotonic_time,
                               double animation_start_time,
                               std::unique_ptr<cc::AnimationCurve>) override;

  std::unique_ptr<CompositorScrollOffsetAnimationCurve> animation_curve_;
  double start_time_;
  WTF::TimeFunction time_function_;

 private:
  // Returns true if the animation was scheduled successfully. If animation
  // could not be scheduled (e.g. because the frame is detached), scrolls
  // immediately to the target and returns false.
  bool RegisterAndScheduleAnimation();

  void CreateAnimationCurve();
  void PostAnimationCleanupAndReset();

  void AddMainThreadScrollingReason();
  void RemoveMainThreadScrollingReason();

  // Returns true if will animate to the given target offset. Returns false
  // only when there is no animation running and we are not starting one
  // because we are already at targetPos.
  bool WillAnimateToOffset(const ScrollOffset& target_pos);

  ScrollOffset target_offset_;
  ScrollGranularity last_granularity_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SCROLL_SCROLL_ANIMATOR_H_
