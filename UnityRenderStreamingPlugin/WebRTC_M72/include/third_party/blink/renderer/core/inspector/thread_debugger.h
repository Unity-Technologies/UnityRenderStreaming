// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_INSPECTOR_THREAD_DEBUGGER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_INSPECTOR_THREAD_DEBUGGER_H_

#include <memory>
#include "base/macros.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/user_gesture_indicator.h"
#include "third_party/blink/renderer/core/inspector/console_types.h"
#include "third_party/blink/renderer/platform/bindings/v8_per_isolate_data.h"
#include "third_party/blink/renderer/platform/cross_thread_copier.h"
#include "third_party/blink/renderer/platform/timer.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"
#include "v8/include/v8-inspector.h"
#include "v8/include/v8.h"

namespace blink {

class ExecutionContext;
class SourceLocation;

class CORE_EXPORT ThreadDebugger : public v8_inspector::V8InspectorClient,
                                   public V8PerIsolateData::Data {
 public:
  explicit ThreadDebugger(v8::Isolate*);
  ~ThreadDebugger() override;

  static ThreadDebugger* From(v8::Isolate*);
  virtual bool IsWorker() = 0;
  v8_inspector::V8Inspector* GetV8Inspector() const {
    return v8_inspector_.get();
  }

  static void IdleStarted(v8::Isolate*);
  static void IdleFinished(v8::Isolate*);

  void AsyncTaskScheduled(const StringView& task_name,
                          void* task,
                          bool recurring);
  void AsyncTaskCanceled(void* task);
  void AllAsyncTasksCanceled();
  void AsyncTaskStarted(void* task);
  void AsyncTaskFinished(void* task);
  unsigned PromiseRejected(v8::Local<v8::Context>,
                           const String& error_message,
                           v8::Local<v8::Value> exception,
                           std::unique_ptr<SourceLocation>);
  void PromiseRejectionRevoked(v8::Local<v8::Context>,
                               unsigned promise_rejection_id);

  v8_inspector::V8StackTraceId StoreCurrentStackTrace(
      const StringView& description);
  void ExternalAsyncTaskStarted(const v8_inspector::V8StackTraceId& parent);
  void ExternalAsyncTaskFinished(const v8_inspector::V8StackTraceId& parent);

 protected:
  virtual int ContextGroupId(ExecutionContext*) = 0;
  virtual void ReportConsoleMessage(ExecutionContext*,
                                    MessageSource,
                                    MessageLevel,
                                    const String& message,
                                    SourceLocation*) = 0;
  void installAdditionalCommandLineAPI(v8::Local<v8::Context>,
                                       v8::Local<v8::Object>) override;
  void CreateFunctionProperty(v8::Local<v8::Context>,
                              v8::Local<v8::Object>,
                              const char* name,
                              v8::FunctionCallback,
                              const char* description,
                              v8::SideEffectType side_effect_type);
  static v8::Maybe<bool> CreateDataPropertyInArray(v8::Local<v8::Context>,
                                                   v8::Local<v8::Array>,
                                                   int index,
                                                   v8::Local<v8::Value>);
  static MessageLevel V8MessageLevelToMessageLevel(
      v8::Isolate::MessageErrorLevel);

  v8::Isolate* isolate_;

 private:
  // V8InspectorClient implementation.
  void beginUserGesture() override;
  void endUserGesture() override;
  std::unique_ptr<v8_inspector::StringBuffer> valueSubtype(
      v8::Local<v8::Value>) override;
  bool formatAccessorsAsProperties(v8::Local<v8::Value>) override;
  double currentTimeMS() override;
  bool isInspectableHeapObject(v8::Local<v8::Object>) override;
  void consoleTime(const v8_inspector::StringView& title) override;
  void consoleTimeEnd(const v8_inspector::StringView& title) override;
  void consoleTimeStamp(const v8_inspector::StringView& title) override;
  void startRepeatingTimer(double,
                           v8_inspector::V8InspectorClient::TimerCallback,
                           void* data) override;
  void cancelTimer(void* data) override;

  void OnTimer(TimerBase*);

  static void SetMonitorEventsCallback(
      const v8::FunctionCallbackInfo<v8::Value>&,
      bool enabled);
  static void MonitorEventsCallback(const v8::FunctionCallbackInfo<v8::Value>&);
  static void UnmonitorEventsCallback(
      const v8::FunctionCallbackInfo<v8::Value>&);

  static void GetEventListenersCallback(
      const v8::FunctionCallbackInfo<v8::Value>&);

  std::unique_ptr<v8_inspector::V8Inspector> v8_inspector_;
  Vector<std::unique_ptr<TaskRunnerTimer<ThreadDebugger>>> timers_;
  Vector<v8_inspector::V8InspectorClient::TimerCallback> timer_callbacks_;
  Vector<void*> timer_data_;
  std::unique_ptr<UserGestureIndicator> user_gesture_indicator_;
  DISALLOW_COPY_AND_ASSIGN(ThreadDebugger);
};

template <>
struct CrossThreadCopier<v8_inspector::V8StackTraceId> {
  typedef v8_inspector::V8StackTraceId Type;
  static Type Copy(const Type& id) { return id; }
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_INSPECTOR_THREAD_DEBUGGER_H_
