// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file contains the Blink version of RuntimeCallStats which is implemented
// by V8 in //v8/src/counters.h

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_RUNTIME_CALL_STATS_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_RUNTIME_CALL_STATS_H_

#include "base/optional.h"
#include "third_party/blink/renderer/platform/bindings/runtime_call_stats_count_everything_buildflags.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/runtime_enabled_features.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/time.h"
#include "v8/include/v8.h"

#if BUILDFLAG(RCS_COUNT_EVERYTHING)
#include "third_party/blink/renderer/platform/wtf/hash_map.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"
#endif

namespace base {
class TickClock;
}

namespace blink {

class TracedValue;

// A simple counter used to track total execution count & time for a particular
// function/scope.
class PLATFORM_EXPORT RuntimeCallCounter {
 public:
  explicit RuntimeCallCounter(const char* name) : count_(0), name_(name) {}

  void IncrementAndAddTime(TimeDelta time) {
    count_++;
    time_ += time;
  }

  uint64_t GetCount() const { return count_; }
  TimeDelta GetTime() const { return time_; }
  const char* GetName() const { return name_; }

  void Reset() {
    time_ = TimeDelta();
    count_ = 0;
  }

  void Dump(TracedValue&) const;

 private:
  RuntimeCallCounter() = default;

  uint64_t count_;
  TimeDelta time_;
  const char* name_;

  friend class RuntimeCallStats;
};

// Used to track elapsed time for a counter.
// NOTE: Do not use this class directly to track execution times, instead use it
// with the macros below.
class PLATFORM_EXPORT RuntimeCallTimer {
 public:
  explicit RuntimeCallTimer(const base::TickClock* clock) : clock_(clock) {}
  ~RuntimeCallTimer() { DCHECK(!IsRunning()); };

  // Starts recording time for <counter>, and pauses <parent> (if non-null).
  void Start(RuntimeCallCounter*, RuntimeCallTimer* parent);

  // Stops recording time for the counter passed in Start(), and also updates
  // elapsed time and increments the count stored by the counter. It also
  // resumes the parent timer passed in Start() (if any).
  RuntimeCallTimer* Stop();

  // Resets the timer. Call this before reusing a timer.
  void Reset() {
    start_ticks_ = TimeTicks();
    elapsed_time_ = TimeDelta();
  }

 private:
  void Pause(TimeTicks now) {
    DCHECK(IsRunning());
    elapsed_time_ += (now - start_ticks_);
    start_ticks_ = TimeTicks();
  }

  void Resume(TimeTicks now) {
    DCHECK(!IsRunning());
    start_ticks_ = now;
  }

  bool IsRunning() { return start_ticks_ != TimeTicks(); }

  RuntimeCallCounter* counter_;
  RuntimeCallTimer* parent_;
  TimeTicks start_ticks_;
  TimeDelta elapsed_time_;
  const base::TickClock* clock_ = nullptr;
};

// Macros that take RuntimeCallStats as a parameter; used only in
// RuntimeCallStatsTest.
#define RUNTIME_CALL_STATS_ENTER_WITH_RCS(runtime_call_stats, timer,      \
                                          counterId)                      \
  if (UNLIKELY(RuntimeEnabledFeatures::BlinkRuntimeCallStatsEnabled())) { \
    (runtime_call_stats)->Enter(timer, counterId);                        \
  }

#define RUNTIME_CALL_STATS_LEAVE_WITH_RCS(runtime_call_stats, timer)      \
  if (UNLIKELY(RuntimeEnabledFeatures::BlinkRuntimeCallStatsEnabled())) { \
    (runtime_call_stats)->Leave(timer);                                   \
  }

#define RUNTIME_CALL_TIMER_SCOPE_WITH_RCS(runtime_call_stats, counterId)  \
  base::Optional<RuntimeCallTimerScope> rcs_scope;                        \
  if (UNLIKELY(RuntimeEnabledFeatures::BlinkRuntimeCallStatsEnabled())) { \
    rcs_scope.emplace(runtime_call_stats, counterId);                     \
  }

#define RUNTIME_CALL_TIMER_SCOPE_WITH_OPTIONAL_RCS(                       \
    optional_scope_name, runtime_call_stats, counterId)                   \
  if (UNLIKELY(RuntimeEnabledFeatures::BlinkRuntimeCallStatsEnabled())) { \
    optional_scope_name.emplace(runtime_call_stats, counterId);           \
  }

// Use the macros below instead of directly using RuntimeCallStats::Enter,
// RuntimeCallStats::Leave and RuntimeCallTimerScope. They force an early
// exit if Runtime Call Stats is disabled.
#define RUNTIME_CALL_STATS_ENTER(isolate, timer, counterId)                 \
  RUNTIME_CALL_STATS_ENTER_WITH_RCS(RuntimeCallStats::From(isolate), timer, \
                                    counterId)

#define RUNTIME_CALL_STATS_LEAVE(isolate, timer) \
  RUNTIME_CALL_STATS_LEAVE_WITH_RCS(RuntimeCallStats::From(isolate), timer)

#define RUNTIME_CALL_TIMER_SCOPE(isolate, counterId) \
  RUNTIME_CALL_TIMER_SCOPE_WITH_RCS(RuntimeCallStats::From(isolate), counterId)

#define RUNTIME_CALL_TIMER_SCOPE_IF_ISOLATE_EXISTS(isolate, counterId) \
  base::Optional<RuntimeCallTimerScope> rcs_scope;                     \
  if (isolate) {                                                       \
    RUNTIME_CALL_TIMER_SCOPE_WITH_OPTIONAL_RCS(                        \
        rcs_scope, RuntimeCallStats::From(isolate), counterId)         \
  }

// Used in places which do not have a counter explicitly defined in
// FOR_EACH_COUNTER. This is a no-op by default (when RCS_COUNT_EVERYTHING is
// not set).
#if BUILDFLAG(RCS_COUNT_EVERYTHING)
#define RUNTIME_CALL_TIMER_SCOPE_DISABLED_BY_DEFAULT(isolate, counterName) \
  RUNTIME_CALL_TIMER_SCOPE(isolate, counterName)
#else
#define RUNTIME_CALL_TIMER_SCOPE_DISABLED_BY_DEFAULT(isolate, counterName) \
  do {                                                                     \
  } while (false)
#endif

// Maintains a stack of timers and provides functions to manage recording scopes
// by pausing and resuming timers in the chain when entering and leaving a
// scope.
class PLATFORM_EXPORT RuntimeCallStats {
 public:
  explicit RuntimeCallStats(const base::TickClock*);
  // Get RuntimeCallStats object associated with the given isolate.
  static RuntimeCallStats* From(v8::Isolate*);

// The following 3 macros are used to define counters that are used in the
// bindings layer to measure call stats for IDL interface methods and
// attributes. Also see documentation for [RuntimeCallStatsCounter] in
// bindings/IDLExtendedAttributes.md.

// Use this to define a counter for IDL interface methods.
// [RuntimeCallStatsCounter=MethodCounter] void method() =>
// BINDINGS_METHOD(V, MethodCounter)
#define BINDINGS_METHOD(V, counter) V(counter)

// Use this to define a counter for IDL readonly attributes.
// [RuntimeCallStatsCounter=AttributeCounter] readonly attribute boolean attr =>
// BINDINGS_READ_ONLY_ATTRIBUTE(V, AttributeCounter)
#define BINDINGS_READ_ONLY_ATTRIBUTE(V, counter) V(counter##_Getter)

// Use this to define counters for IDL attributes (defines a counter each for
// getter and setter).
// [RuntimeCallStats=AttributeCounter] attribute long attr
// => BINDINGS_ATTRIBUTE(V, AttributeCounter)
#define BINDINGS_ATTRIBUTE(V, counter) \
  V(counter##_Getter)                  \
  V(counter##_Setter)

// Counters

#define BINDINGS_COUNTERS(V)      \
  V(AssociateObjectWithWrapper)   \
  V(CreateWrapper)                \
  V(GetEventListener)             \
  V(HasInstance)                  \
  V(ToExecutionContext)           \
  V(ToV8DOMWindow)                \
  V(ToV8SequenceInternal)         \
  V(SetReturnValueFromStringSlow) \
  V(V8ExternalStringSlow)

#define GC_COUNTERS(V) \
  V(CollectGarbage)    \
  V(GcEpilogue)        \
  V(GcPrologue)        \
  V(PerformIdleLazySweep)

#define PARSING_COUNTERS(V)      \
  V(DocumentFragmentParseHTML)   \
  V(ParserAppendChild)           \
  V(ReplaceChildrenWithFragment) \
  V(HTMLTokenizerNextToken)      \
  V(ConstructTree)

#define STYLE_COUNTERS(V) \
  V(ProcessStyleSheet)    \
  V(UpdateStyle)

#define LAYOUT_COUNTERS(V) \
  V(UpdateLayout)          \
  V(UpdateLayerPositionsAfterLayout)

#define CALLBACK_COUNTERS(V)                       \
  BINDINGS_METHOD(V, ElementGetBoundingClientRect) \
  BINDINGS_METHOD(V, EventTargetDispatchEvent)     \
  BINDINGS_METHOD(V, HTMLElementClick)             \
  BINDINGS_METHOD(V, NodeAppendChild)              \
  BINDINGS_METHOD(V, NodeRemoveChild)              \
  BINDINGS_METHOD(V, WindowSetTimeout)             \
  BINDINGS_ATTRIBUTE(V, DocumentCookie)            \
  BINDINGS_ATTRIBUTE(V, ElementInnerHTML)          \
  BINDINGS_READ_ONLY_ATTRIBUTE(V, NodeName)

#define EXTRA_COUNTERS(V)                                               \
  V(V8)                                                                 \
  V(TestCounter1)                                                       \
  V(TestCounter2)                                                       \
  BINDINGS_METHOD(V, BindingsMethodTestCounter)                         \
  BINDINGS_READ_ONLY_ATTRIBUTE(V, BindingsReadOnlyAttributeTestCounter) \
  BINDINGS_ATTRIBUTE(V, BindingsAttributeTestCounter)

#define FOR_EACH_COUNTER(V) \
  BINDINGS_COUNTERS(V)      \
  GC_COUNTERS(V)            \
  PARSING_COUNTERS(V)       \
  STYLE_COUNTERS(V)         \
  LAYOUT_COUNTERS(V)        \
  CALLBACK_COUNTERS(V)      \
  EXTRA_COUNTERS(V)

  enum class CounterId : uint16_t {
#define ADD_ENUM_VALUE(counter) k##counter,
    FOR_EACH_COUNTER(ADD_ENUM_VALUE)
#undef ADD_ENUM_VALUE
        kNumberOfCounters
  };

  // Enters a new recording scope by pausing the currently running timer that
  // was started by the current instance, and starting <timer>.
  // NOTE: Do not use this function directly, use RUNTIME_CALL_STATS_ENTER.
  void Enter(RuntimeCallTimer* timer, CounterId id) {
    timer->Start(GetCounter(id), current_timer_);
    current_timer_ = timer;
  }

#if BUILDFLAG(RCS_COUNT_EVERYTHING)
  void Enter(RuntimeCallTimer* timer, const char* id) {
    timer->Start(GetCounter(id), current_timer_);
    current_timer_ = timer;
  }
#endif

  // Exits the current recording scope, by stopping <timer> (and updating the
  // counter associated with <timer>) and resuming the timer that was paused
  // before entering the current scope.
  // NOTE: Do not use this function directly, use RUNTIME_CALL_STATS_LEAVE.
  void Leave(RuntimeCallTimer* timer) {
    DCHECK_EQ(timer, current_timer_);
    current_timer_ = timer->Stop();
  }

  // Reset all the counters.
  void Reset();

  void Dump(TracedValue&) const;

  bool InUse() const { return in_use_; }
  void SetInUse(bool in_use) { in_use_ = in_use; }

  RuntimeCallCounter* GetCounter(CounterId id) {
    return &(counters_[static_cast<uint16_t>(id)]);
  }

  String ToString() const;

  static void SetRuntimeCallStatsForTesting();
  static void ClearRuntimeCallStatsForTesting();

#if BUILDFLAG(RCS_COUNT_EVERYTHING)
  // Look up counter in counter map. If counter doesn't exist, a new counter is
  // created and inserted into the map.
  RuntimeCallCounter* GetCounter(const char* name);
#endif

  const base::TickClock* clock() const { return clock_; }

 private:
  RuntimeCallTimer* current_timer_ = nullptr;
  bool in_use_ = false;
  RuntimeCallCounter counters_[static_cast<int>(CounterId::kNumberOfCounters)];
  static const int number_of_counters_ =
      static_cast<int>(CounterId::kNumberOfCounters);
  const base::TickClock* clock_ = nullptr;

#if BUILDFLAG(RCS_COUNT_EVERYTHING)
  typedef HashMap<const char*, std::unique_ptr<RuntimeCallCounter>> CounterMap;
  CounterMap counter_map_;

  Vector<RuntimeCallCounter*> CounterMapToSortedArray() const;
  void AddCounterMapStatsToBuilder(StringBuilder&) const;
#endif
};

// A utility class that creates a RuntimeCallTimer and uses it with
// RuntimeCallStats to measure execution time of a C++ scope.
// Do not use this class directly, use RUNTIME_CALL_TIMER_SCOPE instead.
class PLATFORM_EXPORT RuntimeCallTimerScope {
 public:
  RuntimeCallTimerScope(RuntimeCallStats* stats,
                        RuntimeCallStats::CounterId counter)
      : call_stats_(stats), timer_(stats->clock()) {
    call_stats_->Enter(&timer_, counter);
  }
#if BUILDFLAG(RCS_COUNT_EVERYTHING)
  RuntimeCallTimerScope(RuntimeCallStats* stats, const char* counterName)
      : call_stats_(stats), timer_(stats->clock()) {
    call_stats_->Enter(&timer_, counterName);
  }
#endif
  ~RuntimeCallTimerScope() { call_stats_->Leave(&timer_); }

 private:
  RuntimeCallStats* call_stats_;
  RuntimeCallTimer timer_;
};

// Creates scoped begin and end trace events. The end trace event also contains
// a dump of RuntimeCallStats collected to that point (and the stats are reset
// before sending a begin event). Use this to define regions where
// RuntimeCallStats data is collected and dumped through tracing.
// NOTE: Nested scoped tracers will not send events of their own, the stats
// collected in their scopes will be dumped by the root tracer scope.
class PLATFORM_EXPORT RuntimeCallStatsScopedTracer {
 public:
  explicit RuntimeCallStatsScopedTracer(v8::Isolate* isolate) {
    if (UNLIKELY(RuntimeEnabledFeatures::BlinkRuntimeCallStatsEnabled()))
      AddBeginTraceEventIfEnabled(isolate);
  }

  ~RuntimeCallStatsScopedTracer() {
    if (stats_)
      AddEndTraceEvent();
  }

 private:
  void AddBeginTraceEventIfEnabled(v8::Isolate* isolate);
  void AddEndTraceEvent();

  static const char* const s_category_group_;
  static const char* const s_name_;

  RuntimeCallStats* stats_ = nullptr;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_RUNTIME_CALL_STATS_H_
