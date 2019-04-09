// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_PUBLIC_POST_CROSS_THREAD_TASK_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_PUBLIC_POST_CROSS_THREAD_TASK_H_

#include "base/location.h"
#include "base/sequenced_task_runner.h"
#include "base/time/time.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/functional.h"

namespace blink {

// For cross-thread posting. Can be called from any thread.
PLATFORM_EXPORT void PostCrossThreadTask(base::SequencedTaskRunner&,
                                         const base::Location&,
                                         WTF::CrossThreadClosure);

PLATFORM_EXPORT void PostDelayedCrossThreadTask(base::SequencedTaskRunner&,
                                                const base::Location&,
                                                WTF::CrossThreadClosure,
                                                base::TimeDelta delay);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_PUBLIC_POST_CROSS_THREAD_TASK_H_
