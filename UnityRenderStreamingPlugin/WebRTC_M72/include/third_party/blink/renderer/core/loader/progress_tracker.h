/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_PROGRESS_TRACKER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_PROGRESS_TRACKER_H_

#include <memory>

#include "base/macros.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/loader/frame_loader_types.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource_load_priority.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/hash_map.h"

namespace blink {

class LocalFrameClient;
class LocalFrame;
class ResourceResponse;
struct ProgressItem;

// FIXME: This is only used on Android. Android is the only Chrome
// browser which shows a progress bar during loading.
// We should find a better way for Android to get this data and remove this!
class CORE_EXPORT ProgressTracker final
    : public GarbageCollectedFinalized<ProgressTracker> {
 public:
  static ProgressTracker* Create(LocalFrame*);

  ~ProgressTracker();
  void Trace(blink::Visitor*);
  void Dispose();

  double EstimatedProgress() const;

  void ProgressStarted();
  void ProgressCompleted();

  void FinishedParsing();
  void DidFirstContentfulPaint();

  void WillStartLoading(unsigned long identifier, ResourceLoadPriority);
  void IncrementProgress(unsigned long identifier, const ResourceResponse&);
  void IncrementProgress(unsigned long identifier, int);
  void CompleteProgress(unsigned long identifier);

 private:
  explicit ProgressTracker(LocalFrame*);

  LocalFrameClient* GetLocalFrameClient() const;

  void MaybeSendProgress();
  void SendFinalProgress();
  void Reset();

  bool HaveParsedAndPainted();

  Member<LocalFrame> frame_;
  double last_notified_progress_value_;
  double last_notified_progress_time_;
  bool finished_parsing_;
  bool did_first_contentful_paint_;
  double progress_value_;

  HashMap<unsigned long, std::unique_ptr<ProgressItem>> progress_items_;

  DISALLOW_COPY_AND_ASSIGN(ProgressTracker);
};

}  // namespace blink

#endif
