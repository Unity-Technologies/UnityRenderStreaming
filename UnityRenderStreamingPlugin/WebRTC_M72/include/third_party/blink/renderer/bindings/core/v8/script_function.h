/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_BINDINGS_CORE_V8_SCRIPT_FUNCTION_H_
#define THIRD_PARTY_BLINK_RENDERER_BINDINGS_CORE_V8_SCRIPT_FUNCTION_H_

#include "third_party/blink/renderer/bindings/core/v8/script_value.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "v8/include/v8.h"

namespace blink {

// A common way of using ScriptFunction is as follows:
//
// class DerivedFunction : public ScriptFunction {
//   // This returns a V8 function which the DerivedFunction is bound to.
//   // The DerivedFunction is destroyed when the V8 function is
//   // garbage-collected.
//   static v8::Local<v8::Function> CreateFunction(ScriptState* script_state) {
//     DerivedFunction* self = new DerivedFunction(script_state);
//     return self->BindToV8Function();
//   }
// };
class CORE_EXPORT ScriptFunction
    : public GarbageCollectedFinalized<ScriptFunction> {
 public:
  virtual ~ScriptFunction() = default;
  virtual void Trace(blink::Visitor*);

 protected:
  explicit ScriptFunction(ScriptState* script_state)
      : script_state_(script_state) {}

  ScriptState* GetScriptState() const { return script_state_; }

  v8::Local<v8::Function> BindToV8Function();

 private:
  // Subclasses should implement one of Call() or CallRaw(). Most will implement
  // Call().
  virtual ScriptValue Call(ScriptValue);

  // To support more than one argument, or for low-level access to the V8 API,
  // implement CallRaw(). The default implementation delegates to Call().
  virtual void CallRaw(const v8::FunctionCallbackInfo<v8::Value>&);

  static void CallCallback(const v8::FunctionCallbackInfo<v8::Value>&);

  Member<ScriptState> script_state_;
#if DCHECK_IS_ON()
  // bindToV8Function must not be called twice.
  bool bind_to_v8_function_already_called_ = false;
#endif
};

}  // namespace blink

#endif
