// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_STREAMS_WRITABLE_STREAM_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_STREAMS_WRITABLE_STREAM_H_

#include "third_party/blink/renderer/bindings/core/v8/script_promise.h"
#include "third_party/blink/renderer/bindings/core/v8/script_value.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/bindings/trace_wrapper_v8_reference.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "v8/include/v8.h"

namespace blink {

// This is an implementation of the corresponding IDL interface.
// Use TraceWrapperMember to hold a reference to an instance of this class.
class CORE_EXPORT WritableStream : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  // Call one of Init functions before using the instance.
  WritableStream() = default;
  ~WritableStream() override = default;

  // If an error happens, |exception_state.HadException()| will be true, and
  // |this| will not be usable after that.
  void Init(ScriptState*,
            ScriptValue underlying_sink,
            ScriptValue strategy,
            ExceptionState& exception_state);

  // Create* functions call Init* internally and return null when an error
  // happens.
  static WritableStream* Create(ScriptState*, ExceptionState&);
  static WritableStream* Create(ScriptState*,
                                ScriptValue underlying_sink,
                                ExceptionState&);
  static WritableStream* Create(ScriptState*,
                                ScriptValue underlying_sink,
                                ScriptValue strategy,
                                ExceptionState&);
  static WritableStream* CreateFromInternalStream(
      ScriptState* script_state,
      ScriptValue internal_stream,
      ExceptionState& exception_state) {
    DCHECK(internal_stream.IsObject());
    return CreateFromInternalStream(script_state,
                                    internal_stream.V8Value().As<v8::Object>(),
                                    exception_state);
  }
  static WritableStream* CreateFromInternalStream(
      ScriptState*,
      v8::Local<v8::Object> internal_stream,
      ExceptionState&);

  void Trace(Visitor* visitor) override;

  // IDL defined functions
  bool locked(ScriptState*, ExceptionState&) const;
  ScriptPromise abort(ScriptState*, ExceptionState&);
  ScriptPromise abort(ScriptState*, ScriptValue reason, ExceptionState&);
  ScriptValue getWriter(ScriptState*, ExceptionState&);

  base::Optional<bool> IsLocked(ScriptState*, ExceptionState&) const;

  ScriptValue GetInternalStream(ScriptState*) const;

 private:
  bool InitInternal(ScriptState*, v8::Local<v8::Object> internal_stream);

  static v8::MaybeLocal<v8::Object> CreateInternalStream(
      ScriptState* script_state,
      v8::Local<v8::Value> underlying_sink,
      v8::Local<v8::Value> strategy);

  TraceWrapperV8Reference<v8::Object> internal_stream_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_STREAMS_WRITABLE_STREAM_H_
