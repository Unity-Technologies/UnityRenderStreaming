/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_PUBLIC_THREAD_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_PUBLIC_THREAD_H_

#include <stdint.h>
#include "base/callback_forward.h"
#include "base/macros.h"
#include "base/memory/scoped_refptr.h"
#include "base/threading/thread.h"
#include "third_party/blink/public/platform/web_thread_type.h"
#include "third_party/blink/renderer/platform/platform_export.h"

namespace base {
class SingleThreadTaskRunner;
class TimeTicks;
namespace sequence_manager {
class TaskTimeObserver;
}
}  // namespace base

namespace blink {

class FrameOrWorkerScheduler;
class ThreadScheduler;
class Platform;

// Always an integer value.
typedef uintptr_t PlatformThreadId;

struct PLATFORM_EXPORT ThreadCreationParams {
  explicit ThreadCreationParams(WebThreadType);

  ThreadCreationParams& SetThreadNameForTest(const char* name);

  // Sets a scheduler for the context which was responsible for the creation
  // of this thread.
  ThreadCreationParams& SetFrameOrWorkerScheduler(FrameOrWorkerScheduler*);

  WebThreadType thread_type;
  const char* name;
  FrameOrWorkerScheduler* frame_or_worker_scheduler;  // NOT OWNED
  base::Thread::Options thread_options;
};

// The interface of a thread recognized by Blink.
//
// Deleting the thread blocks until all pending, non-delayed tasks have been
// run.
class PLATFORM_EXPORT Thread {
 public:
  friend class Platform;  // For SetMainThread() and IsSimpleMainThread().
  friend class ScopedMainThreadOverrider;  // For SetMainThread().

  // An IdleTask is expected to complete before the deadline it is passed.
  using IdleTask = base::OnceCallback<void(base::TimeTicks deadline)>;

  // TaskObserver is an observer fired before and after a task is executed.
  using TaskObserver = base::MessageLoop::TaskObserver;

  // Creates a new thread. This may be called from a non-main thread (e.g.
  // nested Web workers).
  static std::unique_ptr<Thread> CreateThread(const ThreadCreationParams&);

  // Creates a WebAudio-specific thread with the elevated priority. Do NOT use
  // for any other purpose.
  static std::unique_ptr<Thread> CreateWebAudioThread();

  // Create and save (as a global variable) the compositor thread. The thread
  // will be accessible through CompositorThread().
  static void CreateAndSetCompositorThread();

  // Return an interface to the current thread.
  static Thread* Current();

  // Return an interface to the main thread.
  static Thread* MainThread();

  // Return an interface to the compositor thread (if initialized). This can be
  // null if the renderer was created with threaded rendering disabled.
  static Thread* CompositorThread();

  Thread();
  virtual ~Thread();

  // Must be called immediately after the construction.
  virtual void Init() {}

  // DEPRECATED: Returns a task runner bound to the underlying scheduler's
  // default task queue.
  //
  // Default scheduler task queue does not give scheduler enough freedom to
  // manage task priorities and should not be used.
  // Use ExecutionContext::GetTaskRunner instead (crbug.com/624696).
  virtual scoped_refptr<base::SingleThreadTaskRunner> GetTaskRunner() const {
    return nullptr;
  }

  bool IsCurrentThread() const;
  virtual PlatformThreadId ThreadId() const { return 0; }

  // TaskObserver is an object that receives task notifications from the
  // MessageLoop.
  // NOTE: TaskObserver implementation should be extremely fast!
  // This API is performance sensitive. Use only if you have a compelling
  // reason.
  void AddTaskObserver(TaskObserver*);
  void RemoveTaskObserver(TaskObserver*);

  // TaskTimeObserver is an object that receives notifications for
  // CPU time spent in each top-level MessageLoop task.
  // NOTE: TaskTimeObserver implementation should be extremely fast!
  // This API is performance sensitive. Use only if you have a compelling
  // reason.
  virtual void AddTaskTimeObserver(base::sequence_manager::TaskTimeObserver*) {}
  virtual void RemoveTaskTimeObserver(
      base::sequence_manager::TaskTimeObserver*) {}

  // Returns the scheduler associated with the thread.
  virtual ThreadScheduler* Scheduler() = 0;

 private:
  // For Platform and ScopedMainThreadOverrider. Return the thread object
  // previously set (if any).
  //
  // This is done this way because we need to be able to "override" the main
  // thread temporarily for ScopedTestingPlatformSupport.
  static std::unique_ptr<Thread> SetMainThread(std::unique_ptr<Thread>);

  // This is used to identify the actual Thread instance. This should be
  // used only in Platform, and other users should ignore this.
  virtual bool IsSimpleMainThread() const { return false; }

  DISALLOW_COPY_AND_ASSIGN(Thread);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_SCHEDULER_PUBLIC_THREAD_H_
