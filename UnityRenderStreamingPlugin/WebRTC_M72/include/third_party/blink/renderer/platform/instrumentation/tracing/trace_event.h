// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_INSTRUMENTATION_TRACING_TRACE_EVENT_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_INSTRUMENTATION_TRACING_TRACE_EVENT_H_

#include "base/time/time.h"
#include "base/trace_event/trace_event.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/text/cstring.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace WTF {

// CString version of SetTraceValue so that trace arguments can be strings.
static inline void SetTraceValue(const CString& arg,
                                 unsigned char* type,
                                 unsigned long long* value) {
  trace_event_internal::TraceValueUnion type_value;
  type_value.as_string = arg.data();
  *type = TRACE_VALUE_TYPE_COPY_STRING;
  *value = type_value.as_uint;
}

}  // namespace WTF

namespace blink {
namespace trace_event {

using base::trace_event::TraceScopedTrackableObject;
using AsyncEnabledStateObserver =
    base::trace_event::TraceLog::AsyncEnabledStateObserver;
using EnabledStateObserver = base::trace_event::TraceLog::EnabledStateObserver;

inline base::TimeTicks ToTraceTimestamp(double seconds) {
  return base::TimeTicks() + base::TimeDelta::FromSecondsD(seconds);
}

// This is to avoid error of passing a chromium time internal value.
void ToTraceTimestamp(int64_t);

PLATFORM_EXPORT void EnableTracing(const String& category_filter);
PLATFORM_EXPORT void DisableTracing();

PLATFORM_EXPORT void AddAsyncEnabledStateObserver(
    base::WeakPtr<AsyncEnabledStateObserver>);
PLATFORM_EXPORT void RemoveAsyncEnabledStateObserver(
    AsyncEnabledStateObserver*);

PLATFORM_EXPORT void AddEnabledStateObserver(EnabledStateObserver*);
PLATFORM_EXPORT void RemoveEnabledStateObserver(EnabledStateObserver*);

}  // namespace trace_event
}  // namespace blink

#endif
