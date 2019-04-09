// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_EXPERIMENTAL_TASK_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_EXPERIMENTAL_TASK_H_

#include "third_party/blink/renderer/bindings/core/v8/script_promise_resolver.h"
#include "third_party/blink/renderer/core/workers/experimental/thread_pool_thread.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/wtf/ref_counted.h"
#include "third_party/blink/renderer/platform/wtf/threading_primitives.h"

namespace blink {
class ResolveTask;
class SerializedScriptValue;

// Runs |function| with |arguments| on a thread from the given ThreadPool.
// Scans |arguments| for Task objects, and registers those as dependencies,
// passing the result of those tasks in place of the Task arguments.
// All public functions are main-thread-only.
// TaskBase keeps itself alive via a SelfKeepAlive until the
// the task completes and reports itself done on the main thread via
// TaskCompleted().
class TaskBase : public GarbageCollectedMixin {
 public:
  virtual ~TaskBase();

 protected:
  virtual void StartTaskOnWorkerThread() LOCKS_EXCLUDED(mutex_) = 0;
  virtual bool IsTargetThreadForArguments() = 0;

  enum class State { kPending, kStarted, kCancelPending, kCompleted, kFailed };

  TaskBase(TaskType,
           ScriptState*,
           const ScriptValue& function,
           const String& function_name);

  void InitializeArgumentsOnMainThread(ThreadPoolThreadProvider*,
                                       ScriptState*,
                                       const Vector<ScriptValue>& arguments);

  class AsyncFunctionCompleted;

  // This caches the result after the task completes on the worker thread.
  // We can't safely clear the ScopedPersistent from the main thread, so
  // this wrappper allows us to hold a CrossThreadPersistent that arranges
  // for GC on the worker thread.
  class V8ResultHolder final
      : public GarbageCollectedFinalized<V8ResultHolder> {
   public:
    V8ResultHolder(v8::Isolate* isolate, v8::Local<v8::Value> result)
        : result_(isolate, result) {}
    ~V8ResultHolder() = default;
    v8::Local<v8::Value> GetResult(v8::Isolate* isolate) {
      return result_.NewLocal(isolate);
    }
    void Trace(Visitor*) {}

   private:
    ScopedPersistent<v8::Value> result_;
  };

  bool WillStartTaskOnWorkerThread();
  void RunTaskOnWorkerThread();
  void TaskCompletedOnWorkerThread(v8::Local<v8::Value> v8_result, State)
      LOCKS_EXCLUDED(mutex_);
  void PassResultToDependentOnWorkerThread(size_t dependent_index, TaskBase*)
      LOCKS_EXCLUDED(mutex_);

  // Called on ANY thread (main thread, worker_thread_, or a sibling worker).
  void MaybeStartTask() EXCLUSIVE_LOCKS_REQUIRED(mutex_);
  void PrerequisiteFinished(size_t index,
                            V8ResultHolder*,
                            scoped_refptr<SerializedScriptValue>,
                            bool failed) LOCKS_EXCLUDED(mutex_);
  bool HasFinished() const EXCLUSIVE_LOCKS_REQUIRED(mutex_) {
    return state_ == State::kCompleted || state_ == State::kFailed;
  }
  void AdvanceState(State new_state) EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  // Called on main thread or worker_thread_
  scoped_refptr<SerializedScriptValue> GetSerializedResult()
      LOCKS_EXCLUDED(mutex_);

  // Called on main thread
  static ThreadPoolThread* SelectThread(
      const HeapVector<Member<TaskBase>>& prerequisites,
      ThreadPoolThreadProvider*);
  void RegisterDependencies(const HeapVector<Member<TaskBase>>& prerequisites,
                            const Vector<size_t>& prerequisite_indices)
      LOCKS_EXCLUDED(mutex_);
  virtual void TaskCompleted(bool was_successful);

  // worker_thread_ is selected in the constructor and not changed thereafter.
  ThreadPoolThread* worker_thread_ = nullptr;
  const TaskType task_type_;

  // Main thread only
  SelfKeepAlive<TaskBase> self_keep_alive_;

  // Created in constructor on the main thread, consumed and cleared on
  // worker_thread_. Those steps can't overlap, so no mutex_ required.
  scoped_refptr<SerializedScriptValue> function_;
  const String function_name_;

  // Created and populated with non-prerequiste parameters on the main thread.
  // Each prerequisite writes its return value into arguments_ from its thread.
  // If the prequisite and this have the same worker_thread_, there is no need
  // to serialize and deserialize the argument, so v8_value will be populated
  // with the v8::Value returned by the prerequisite.
  // Consumed and cleared on worker_thread_.
  // Only requires mutex_ when writing prerequisite results, at other times
  // either the main thread or the worker_thread_ has sole access.
  struct Argument {
    scoped_refptr<SerializedScriptValue> serialized_value;
    CrossThreadPersistent<V8ResultHolder> v8_value;
  };
  Vector<Argument> arguments_;

  // Read on main thread, write on worker_thread_.
  scoped_refptr<SerializedScriptValue> serialized_result_ GUARDED_BY(mutex_);

  // Read/write on worker_thread_
  CrossThreadPersistent<V8ResultHolder> v8_result_;

  // Read/write on both main thread and worker_thread_.
  State state_ GUARDED_BY(mutex_) = State::kPending;

  // Initialized in constructor on main thread, each completed prerequisite
  // decrements from the prerequisite's thread or main thread.
  size_t prerequisites_remaining_ GUARDED_BY(mutex_) = 0u;

  // Elements added from main thread. Cleared on completion on worker_thread_.
  // Each element in dependents_ is not yet in the kCompleted state.
  struct Dependent final : public GarbageCollected<Dependent> {
   public:
    Dependent(TaskBase* task, size_t index) : task(task), index(index) {
      DCHECK(IsMainThread());
    }
    void Trace(Visitor* visitor) { visitor->Trace(task); }
    Member<TaskBase> task;
    // The index in the dependent's argument array where this result should go.
    size_t index;
  };
  Vector<CrossThreadPersistent<Dependent>> dependents_ GUARDED_BY(mutex_);

  Mutex mutex_;
};

// The variant of TaskBase that is exposed to JS.
class Task final : public ScriptWrappable, public TaskBase {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(Task);

 public:
  // Called on main thread
  Task(ThreadPoolThreadProvider* thread_provider,
       ScriptState* script_state,
       const ScriptValue& function,
       const Vector<ScriptValue>& arguments,
       TaskType task_type)
      : TaskBase(task_type, script_state, function, String()) {
    InitializeArgumentsOnMainThread(thread_provider, script_state, arguments);
  }
  Task(ThreadPoolThreadProvider* thread_provider,
       ScriptState* script_state,
       const String& function_name,
       const Vector<ScriptValue>& arguments,
       TaskType task_type)
      : TaskBase(task_type, script_state, ScriptValue(), function_name) {
    InitializeArgumentsOnMainThread(thread_provider, script_state, arguments);
  }

  // Returns a promise that will be resolved with the result when it completes.
  ScriptPromise result(ScriptState*);
  void cancel() LOCKS_EXCLUDED(mutex_);

  void StartTaskOnWorkerThread() override LOCKS_EXCLUDED(mutex_);
  bool IsTargetThreadForArguments() override {
    return worker_thread_->IsCurrentThread();
  }
  void Trace(Visitor*) override;

 private:
  Member<ResolveTask> resolve_task_;
};

// An internal TaskBase subclass that drives main thread promise resolution.
// It registers itself as a dependent on the Task whose result is being
// promised. When that Task completes, it runs a dummy script that just returns
// the dependent's result as its own. It then eagerly serializes the result, and
// overrides TaskCompleted() to actually resolve the promise.
class ResolveTask final : public GarbageCollectedFinalized<ResolveTask>,
                          public TaskBase {
  USING_GARBAGE_COLLECTED_MIXIN(ResolveTask);

 public:
  ResolveTask(ScriptState*, TaskType, Task* prerequisite);
  void StartTaskOnWorkerThread() override LOCKS_EXCLUDED(mutex_);
  bool IsTargetThreadForArguments() override { return IsMainThread(); }
  void TaskCompleted(bool was_successful) override;
  ScriptPromise GetPromise() { return resolver_->Promise(); }
  void Trace(Visitor*) override;

 private:
  Member<ScriptPromiseResolver> resolver_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_EXPERIMENTAL_TASK_H_
