/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_TIMER_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_TIMER_H_

#include "base/location.h"
#include "base/memory/weak_ptr.h"
#include "base/single_thread_task_runner.h"
#include "base/time/time.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/address_sanitizer.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/noncopyable.h"
#include "third_party/blink/renderer/platform/wtf/threading.h"
#include "third_party/blink/renderer/platform/wtf/time.h"

namespace blink {

// Time intervals are all in seconds.

class PLATFORM_EXPORT TimerBase {
  WTF_MAKE_NONCOPYABLE(TimerBase);

 public:
  explicit TimerBase(scoped_refptr<base::SingleThreadTaskRunner>);
  virtual ~TimerBase();

  void Start(TimeDelta next_fire_interval,
             TimeDelta repeat_interval,
             const base::Location&);

  void StartRepeating(TimeDelta repeat_interval, const base::Location& caller) {
    Start(repeat_interval, repeat_interval, caller);
  }

  void StartOneShot(TimeDelta interval, const base::Location& caller) {
    Start(interval, TimeDelta(), caller);
  }

  // Timer cancellation is fast enough that you shouldn't have to worry
  // about it unless you're canceling tens of thousands of tasks.
  virtual void Stop();
  bool IsActive() const;
  const base::Location& GetLocation() const { return location_; }

  TimeDelta NextFireInterval() const;
  TimeDelta RepeatInterval() const { return repeat_interval_; }

  void AugmentRepeatInterval(TimeDelta delta) {
    TimeTicks now = TimerCurrentTimeTicks();
    SetNextFireTime(now, std::max(next_fire_time_ - now + delta, TimeDelta()));
    repeat_interval_ += delta;
  }

  void MoveToNewTaskRunner(scoped_refptr<base::SingleThreadTaskRunner>);

  struct PLATFORM_EXPORT Comparator {
    bool operator()(const TimerBase* a, const TimerBase* b) const;
  };

 private:
  virtual void Fired() = 0;

  virtual scoped_refptr<base::SingleThreadTaskRunner> TimerTaskRunner() const;

  NO_SANITIZE_ADDRESS
  virtual bool CanFire() const { return true; }

  TimeTicks TimerCurrentTimeTicks() const;

  void SetNextFireTime(TimeTicks now, TimeDelta delay);

  void RunInternal();

  TimeTicks next_fire_time_;   // 0 if inactive
  TimeDelta repeat_interval_;  // 0 if not repeating
  base::Location location_;
  scoped_refptr<base::SingleThreadTaskRunner> web_task_runner_;

#if DCHECK_IS_ON()
  ThreadIdentifier thread_;
#endif
  base::WeakPtrFactory<TimerBase> weak_ptr_factory_;

  friend class ThreadTimers;
  friend class TimerHeapLessThanFunction;
  friend class TimerHeapReference;
};

template <typename T, bool = IsGarbageCollectedType<T>::value>
class TimerIsObjectAliveTrait {
 public:
  static bool IsHeapObjectAlive(T*) { return true; }
};

template <typename T>
class TimerIsObjectAliveTrait<T, true> {
 public:
  static bool IsHeapObjectAlive(T* object_pointer) {
    return !ThreadHeap::WillObjectBeLazilySwept(object_pointer);
  }
};

template <typename TimerFiredClass>
class TaskRunnerTimer : public TimerBase {
 public:
  using TimerFiredFunction = void (TimerFiredClass::*)(TimerBase*);

  TaskRunnerTimer(scoped_refptr<base::SingleThreadTaskRunner> web_task_runner,
                  TimerFiredClass* o,
                  TimerFiredFunction f)
      : TimerBase(std::move(web_task_runner)), object_(o), function_(f) {}

  ~TaskRunnerTimer() override = default;

 protected:
  void Fired() override { (object_->*function_)(this); }

  NO_SANITIZE_ADDRESS
  bool CanFire() const override {
    // Oilpan: if a timer fires while Oilpan heaps are being lazily
    // swept, it is not safe to proceed if the object is about to
    // be swept (and this timer will be stopped while doing so.)
    return TimerIsObjectAliveTrait<TimerFiredClass>::IsHeapObjectAlive(object_);
  }

 private:
  // FIXME: Oilpan: TimerBase should be moved to the heap and m_object should be
  // traced.  This raw pointer is safe as long as Timer<X> is held by the X
  // itself (That's the case
  // in the current code base).
  GC_PLUGIN_IGNORE("363031")
  TimerFiredClass* object_;
  TimerFiredFunction function_;
};

NO_SANITIZE_ADDRESS
inline bool TimerBase::IsActive() const {
#if DCHECK_IS_ON()
  DCHECK_EQ(thread_, CurrentThread());
#endif
  return weak_ptr_factory_.HasWeakPtrs();
}

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_TIMER_H_
