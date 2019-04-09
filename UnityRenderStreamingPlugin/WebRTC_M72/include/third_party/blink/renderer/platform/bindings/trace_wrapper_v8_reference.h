// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_TRACE_WRAPPER_V8_REFERENCE_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_TRACE_WRAPPER_V8_REFERENCE_H_

#include "third_party/blink/renderer/platform/bindings/script_wrappable_marking_visitor.h"
#include "third_party/blink/renderer/platform/heap/unified_heap_marking_visitor.h"

namespace blink {

/**
 * TraceWrapperV8Reference is used to trace from Blink to V8. The reference is
 * (strongly) traced by wrapper tracing.
 *
 * TODO(mlippautz): Use a better handle type than v8::Persistent.
 */
template <typename T>
class TraceWrapperV8Reference {
 public:
  TraceWrapperV8Reference() = default;

  TraceWrapperV8Reference(v8::Isolate* isolate, v8::Local<T> handle) {
    InternalSet(isolate, handle);
    handle_.SetWeak();
  }

  ~TraceWrapperV8Reference() { Clear(); }

  bool operator==(const TraceWrapperV8Reference& other) const {
    return handle_ == other.handle_;
  }

  void Set(v8::Isolate* isolate, v8::Local<T> handle) {
    InternalSet(isolate, handle);
    handle_.SetWeak();
  }

  template <typename P>
  void Set(v8::Isolate* isolate,
           v8::Local<T> handle,
           P* parameters,
           void (*callback)(const v8::WeakCallbackInfo<P>&),
           v8::WeakCallbackType type = v8::WeakCallbackType::kParameter) {
    InternalSet(isolate, handle);
    handle_.SetWeak(parameters, callback, type);
  }

  ALWAYS_INLINE v8::Local<T> NewLocal(v8::Isolate* isolate) const {
    return v8::Local<T>::New(isolate, handle_);
  }

  bool IsEmpty() const { return handle_.IsEmpty(); }
  void Clear() { handle_.Reset(); }
  ALWAYS_INLINE const v8::Global<T>& Get() const { return handle_; }
  ALWAYS_INLINE v8::Global<T>& Get() { return handle_; }

  template <typename S>
  const TraceWrapperV8Reference<S>& Cast() const {
    static_assert(std::is_base_of<S, T>::value, "T must inherit from S");
    return reinterpret_cast<const TraceWrapperV8Reference<S>&>(
        const_cast<const TraceWrapperV8Reference<T>&>(*this));
  }

  template <typename S>
  const TraceWrapperV8Reference<S>& UnsafeCast() const {
    return reinterpret_cast<const TraceWrapperV8Reference<S>&>(
        const_cast<const TraceWrapperV8Reference<T>&>(*this));
  }

 private:
  inline void InternalSet(v8::Isolate* isolate, v8::Local<T> handle) {
    handle_.Reset(isolate, handle);
    ScriptWrappableMarkingVisitor::WriteBarrier(isolate,
                                                UnsafeCast<v8::Value>());
    UnifiedHeapMarkingVisitor::WriteBarrier(isolate, UnsafeCast<v8::Value>());
  }

  v8::Global<T> handle_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_TRACE_WRAPPER_V8_REFERENCE_H_
