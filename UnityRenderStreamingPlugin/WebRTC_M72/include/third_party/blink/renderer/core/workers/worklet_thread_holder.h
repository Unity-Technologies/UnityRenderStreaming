// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_WORKLET_THREAD_HOLDER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_WORKLET_THREAD_HOLDER_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/workers/worker_backing_thread.h"
#include "third_party/blink/renderer/core/workers/worker_backing_thread_startup_data.h"
#include "third_party/blink/renderer/platform/waitable_event.h"
#include "third_party/blink/renderer/platform/web_thread_supporting_gc.h"

namespace blink {

// WorkletThreadHolder is a template class which is designed for singleton
// instance of DerivedWorkletThread (i.e. AnimationWorkletThread,
// AudioWorkletThread).
template <class DerivedWorkletThread>
class WorkletThreadHolder {
 public:
  static WorkletThreadHolder<DerivedWorkletThread>* GetInstance() {
    MutexLocker locker(HolderInstanceMutex());
    return thread_holder_instance_;
  }

  static void EnsureInstance(const ThreadCreationParams& params) {
    DCHECK(IsMainThread());
    MutexLocker locker(HolderInstanceMutex());
    if (thread_holder_instance_)
      return;
    thread_holder_instance_ = new WorkletThreadHolder<DerivedWorkletThread>;
    thread_holder_instance_->Initialize(WorkerBackingThread::Create(params));
  }

  static void ClearInstance() {
    DCHECK(IsMainThread());
    MutexLocker locker(HolderInstanceMutex());
    if (thread_holder_instance_) {
      thread_holder_instance_->ShutdownAndWait();
      delete thread_holder_instance_;
      thread_holder_instance_ = nullptr;
    }
  }

  WorkerBackingThread* GetThread() { return thread_.get(); }

 private:
  WorkletThreadHolder() = default;
  ~WorkletThreadHolder() = default;

  static Mutex& HolderInstanceMutex() {
    DEFINE_THREAD_SAFE_STATIC_LOCAL(Mutex, holder_mutex, ());
    return holder_mutex;
  }

  void Initialize(std::unique_ptr<WorkerBackingThread> backing_thread) {
    thread_ = std::move(backing_thread);
    thread_->BackingThread().PostTask(
        FROM_HERE,
        CrossThreadBind(&WorkletThreadHolder::InitializeOnWorkletThread,
                        CrossThreadUnretained(this)));
  }

  void InitializeOnWorkletThread() {
    MutexLocker locker(HolderInstanceMutex());
    thread_->InitializeOnBackingThread(
        WorkerBackingThreadStartupData::CreateDefault());
  }

  void ShutdownAndWait() {
    DCHECK(IsMainThread());
    WaitableEvent waitable_event;
    thread_->BackingThread().PostTask(
        FROM_HERE,
        CrossThreadBind(&WorkletThreadHolder::ShutdownOnWorkletThread,
                        CrossThreadUnretained(this),
                        CrossThreadUnretained(&waitable_event)));
    waitable_event.Wait();
  }

  void ShutdownOnWorkletThread(WaitableEvent* waitable_event) {
    thread_->ShutdownOnBackingThread();
    waitable_event->Signal();
  }

  std::unique_ptr<WorkerBackingThread> thread_;

  static WorkletThreadHolder<DerivedWorkletThread>* thread_holder_instance_;
};

template <class DerivedWorkletThread>
WorkletThreadHolder<DerivedWorkletThread>*
    WorkletThreadHolder<DerivedWorkletThread>::thread_holder_instance_ =
        nullptr;

}  // namespace blink

#endif  // WorkletBackingThreadHolder_h
