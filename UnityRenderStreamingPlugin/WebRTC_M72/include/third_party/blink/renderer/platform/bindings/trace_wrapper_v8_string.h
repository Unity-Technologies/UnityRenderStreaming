// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_TRACE_WRAPPER_V8_STRING_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_TRACE_WRAPPER_V8_STRING_H_

#include "third_party/blink/renderer/platform/bindings/name_client.h"
#include "third_party/blink/renderer/platform/bindings/trace_wrapper_v8_reference.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "v8/include/v8.h"

namespace blink {

// Small shim around TraceWrapperReference<v8::String> with a few
// utility methods. Internally, v8::String is represented as string
// rope.
class GC_PLUGIN_IGNORE("crbug.com/841830")
    PLATFORM_EXPORT TraceWrapperV8String final : public NameClient {
  DISALLOW_COPY_AND_ASSIGN(TraceWrapperV8String);
  DISALLOW_NEW();

 public:
  TraceWrapperV8String() = default;

  bool IsEmpty() const { return string_.IsEmpty(); }
  void Clear() { string_.Clear(); }

  v8::Local<v8::String> V8Value(v8::Isolate* isolate) {
    return string_.NewLocal(isolate);
  }

  void Concat(v8::Isolate*, const String&);
  String Flatten(v8::Isolate*) const;

  virtual void Trace(Visitor* visitor) { visitor->Trace(string_); }

  const char* NameInHeapSnapshot() const override {
    return "TraceWrapperV8String";
  }

 private:
  TraceWrapperV8Reference<v8::String> string_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_TRACE_WRAPPER_V8_STRING_H_
