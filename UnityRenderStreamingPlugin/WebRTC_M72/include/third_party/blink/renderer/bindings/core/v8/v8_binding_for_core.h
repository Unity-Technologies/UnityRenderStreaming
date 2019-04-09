/*
 * Copyright (C) 2017 Google Inc. All rights reserved.
 * Copyright (C) 2012 Ericsson AB. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_BINDINGS_CORE_V8_V8_BINDING_FOR_CORE_H_
#define THIRD_PARTY_BLINK_RENDERER_BINDINGS_CORE_V8_V8_BINDING_FOR_CORE_H_

#include "third_party/blink/renderer/bindings/core/v8/native_value_traits.h"
#include "third_party/blink/renderer/bindings/core/v8/script_value.h"
#include "third_party/blink/renderer/bindings/core/v8/to_v8_for_core.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_script_runner.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_string_resource.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/node.h"
#include "third_party/blink/renderer/core/typed_arrays/array_buffer_view_helpers.h"
#include "third_party/blink/renderer/platform/bindings/dom_data_store.h"
#include "third_party/blink/renderer/platform/bindings/dom_wrapper_world.h"
#include "third_party/blink/renderer/platform/bindings/exception_state.h"
#include "third_party/blink/renderer/platform/bindings/script_state.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/bindings/v8_binding.h"
#include "third_party/blink/renderer/platform/bindings/v8_binding_macros.h"
#include "third_party/blink/renderer/platform/bindings/v8_per_isolate_data.h"
#include "third_party/blink/renderer/platform/bindings/v8_throw_exception.h"
#include "third_party/blink/renderer/platform/bindings/v8_value_cache.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/text/atomic_string.h"
#include "third_party/blink/renderer/platform/wtf/text/string_view.h"
#include "v8/include/v8.h"

namespace blink {

// This file contains core-specific bindings utility functions. For functions
// that are core independent, see platform/bindings/V8Binding.h. When adding a
// new utility function, consider adding it to V8Binding.h instead unless it has
// dependencies to core/.

class DOMWindow;
class EventTarget;
class ExceptionState;
class ExecutionContext;
class FlexibleArrayBufferView;
class Frame;
class LocalDOMWindow;
class LocalFrame;
class XPathNSResolver;

// Determines how a V8 -> C++ union conversion should be performed: when the
// JavaScript value being converted is either undefined or null, kNullable will
// stop the conversion attempt and the union's IsNull() method will return true.
// If kNotNullable is used, the other conversion steps listed in
// https://heycam.github.io/webidl/#es-union will continue being attempted.
enum class UnionTypeConversionMode {
  kNullable,
  kNotNullable,
};

template <typename CallbackInfo>
inline void V8SetReturnValue(const CallbackInfo& callback_info,
                             DOMWindow* impl) {
  V8SetReturnValue(callback_info, ToV8(impl, callback_info.Holder(),
                                       callback_info.GetIsolate()));
}

template <typename CallbackInfo>
inline void V8SetReturnValue(const CallbackInfo& callback_info,
                             EventTarget* impl) {
  V8SetReturnValue(callback_info, ToV8(impl, callback_info.Holder(),
                                       callback_info.GetIsolate()));
}

template <typename CallbackInfo>
inline void V8SetReturnValue(const CallbackInfo& callback_info, Node* impl) {
  V8SetReturnValue(callback_info, static_cast<ScriptWrappable*>(impl));
}

template <typename CallbackInfo>
inline void V8SetReturnValueForMainWorld(const CallbackInfo& callback_info,
                                         DOMWindow* impl) {
  V8SetReturnValue(callback_info, ToV8(impl, callback_info.Holder(),
                                       callback_info.GetIsolate()));
}

template <typename CallbackInfo>
inline void V8SetReturnValueForMainWorld(const CallbackInfo& callback_info,
                                         EventTarget* impl) {
  V8SetReturnValue(callback_info, ToV8(impl, callback_info.Holder(),
                                       callback_info.GetIsolate()));
}

template <typename CallbackInfo>
inline void V8SetReturnValueForMainWorld(const CallbackInfo& callback_info,
                                         Node* impl) {
  // Since EventTarget has a special version of ToV8 and V8EventTarget.h
  // defines its own v8SetReturnValue family, which are slow, we need to
  // override them with optimized versions for Node and its subclasses.
  // Without this overload, V8SetReturnValueForMainWorld for Node would be
  // very slow.
  //
  // class hierarchy:
  //     ScriptWrappable <-- EventTarget <--+-- Node <-- ...
  //                                        +-- Window
  // overloads:
  //     V8SetReturnValueForMainWorld(ScriptWrappable*)
  //         Optimized and very fast.
  //     V8SetReturnValueForMainWorld(EventTarget*)
  //         Uses custom ToV8 function and slow.
  //     V8SetReturnValueForMainWorld(Node*)
  //         Optimized and very fast.
  //     V8SetReturnValueForMainWorld(Window*)
  //         Uses custom ToV8 function and slow.
  V8SetReturnValueForMainWorld(callback_info,
                               static_cast<ScriptWrappable*>(impl));
}

template <typename CallbackInfo>
inline void V8SetReturnValueFast(const CallbackInfo& callback_info,
                                 DOMWindow* impl,
                                 const ScriptWrappable*) {
  V8SetReturnValue(callback_info, ToV8(impl, callback_info.Holder(),
                                       callback_info.GetIsolate()));
}

template <typename CallbackInfo>
inline void V8SetReturnValueFast(const CallbackInfo& callback_info,
                                 EventTarget* impl,
                                 const ScriptWrappable*) {
  V8SetReturnValue(callback_info, ToV8(impl, callback_info.Holder(),
                                       callback_info.GetIsolate()));
}

template <typename CallbackInfo>
inline void V8SetReturnValueFast(const CallbackInfo& callback_info,
                                 Node* impl,
                                 const ScriptWrappable* wrappable) {
  V8SetReturnValueFast(callback_info, static_cast<ScriptWrappable*>(impl),
                       wrappable);
}

template <typename CallbackInfo, typename T>
inline void V8SetReturnValue(const CallbackInfo& callbackInfo,
                             NotShared<T> notShared) {
  V8SetReturnValue(callbackInfo, notShared.View());
}

template <typename CallbackInfo, typename T>
inline void V8SetReturnValueFast(const CallbackInfo& callbackInfo,
                                 NotShared<T> notShared,
                                 const ScriptWrappable* wrappable) {
  V8SetReturnValueFast(callbackInfo, notShared.View(), wrappable);
}

// Specialized overload, used by interface indexed property handlers in their
// descriptor callbacks, which need an actual V8 Object with the properties of
// a property descriptor.
CORE_EXPORT void V8SetReturnValue(const v8::PropertyCallbackInfo<v8::Value>&,
                                  const v8::PropertyDescriptor&);

// Conversion flags, used in toIntXX/toUIntXX.
enum IntegerConversionConfiguration {
  kNormalConversion,
  kEnforceRange,
  kClamp
};

// Convert a value to a boolean.
CORE_EXPORT bool ToBooleanSlow(v8::Isolate*,
                               v8::Local<v8::Value>,
                               ExceptionState&);
inline bool ToBoolean(v8::Isolate* isolate,
                      v8::Local<v8::Value> value,
                      ExceptionState& exception_state) {
  if (LIKELY(value->IsBoolean()))
    return value.As<v8::Boolean>()->Value();
  return ToBooleanSlow(isolate, value, exception_state);
}

// Convert a value to a 8-bit signed integer. The conversion fails if the
// value cannot be converted to a number or the range violated per WebIDL:
// http://www.w3.org/TR/WebIDL/#es-byte
CORE_EXPORT int8_t ToInt8(v8::Isolate*,
                          v8::Local<v8::Value>,
                          IntegerConversionConfiguration,
                          ExceptionState&);

// Convert a value to a 8-bit unsigned integer. The conversion fails if the
// value cannot be converted to a number or the range violated per WebIDL:
// http://www.w3.org/TR/WebIDL/#es-octet
CORE_EXPORT uint8_t ToUInt8(v8::Isolate*,
                            v8::Local<v8::Value>,
                            IntegerConversionConfiguration,
                            ExceptionState&);

// Convert a value to a 16-bit signed integer. The conversion fails if the
// value cannot be converted to a number or the range violated per WebIDL:
// http://www.w3.org/TR/WebIDL/#es-short
CORE_EXPORT int16_t ToInt16(v8::Isolate*,
                            v8::Local<v8::Value>,
                            IntegerConversionConfiguration,
                            ExceptionState&);

// Convert a value to a 16-bit unsigned integer. The conversion fails if the
// value cannot be converted to a number or the range violated per WebIDL:
// http://www.w3.org/TR/WebIDL/#es-unsigned-short
CORE_EXPORT uint16_t ToUInt16(v8::Isolate*,
                              v8::Local<v8::Value>,
                              IntegerConversionConfiguration,
                              ExceptionState&);

// Convert a value to a 32-bit signed integer. The conversion fails if the
// value cannot be converted to a number or the range violated per WebIDL:
// http://www.w3.org/TR/WebIDL/#es-long
CORE_EXPORT int32_t ToInt32Slow(v8::Isolate*,
                                v8::Local<v8::Value>,
                                IntegerConversionConfiguration,
                                ExceptionState&);
inline int32_t ToInt32(v8::Isolate* isolate,
                       v8::Local<v8::Value> value,
                       IntegerConversionConfiguration configuration,
                       ExceptionState& exception_state) {
  // Fast case. The value is already a 32-bit integer.
  if (LIKELY(value->IsInt32()))
    return value.As<v8::Int32>()->Value();
  return ToInt32Slow(isolate, value, configuration, exception_state);
}

// Convert a value to a 32-bit unsigned integer. The conversion fails if the
// value cannot be converted to a number or the range violated per WebIDL:
// http://www.w3.org/TR/WebIDL/#es-unsigned-long
CORE_EXPORT uint32_t ToUInt32Slow(v8::Isolate*,
                                  v8::Local<v8::Value>,
                                  IntegerConversionConfiguration,
                                  ExceptionState&);
inline uint32_t ToUInt32(v8::Isolate* isolate,
                         v8::Local<v8::Value> value,
                         IntegerConversionConfiguration configuration,
                         ExceptionState& exception_state) {
  // Fast case. The value is already a 32-bit unsigned integer.
  if (LIKELY(value->IsUint32()))
    return value.As<v8::Uint32>()->Value();

  // Fast case. The value is a 32-bit signed integer with NormalConversion
  // configuration.
  if (LIKELY(value->IsInt32() && configuration == kNormalConversion))
    return value.As<v8::Int32>()->Value();

  return ToUInt32Slow(isolate, value, configuration, exception_state);
}

// Convert a value to a 64-bit signed integer. The conversion fails if the
// value cannot be converted to a number or the range violated per WebIDL:
// http://www.w3.org/TR/WebIDL/#es-long-long
CORE_EXPORT int64_t ToInt64Slow(v8::Isolate*,
                                v8::Local<v8::Value>,
                                IntegerConversionConfiguration,
                                ExceptionState&);
inline int64_t ToInt64(v8::Isolate* isolate,
                       v8::Local<v8::Value> value,
                       IntegerConversionConfiguration configuration,
                       ExceptionState& exception_state) {
  // Clamping not supported for int64_t/long long int. See
  // Source/wtf/MathExtras.h.
  DCHECK_NE(configuration, kClamp);

  // Fast case. The value is a 32-bit integer.
  if (LIKELY(value->IsInt32()))
    return value.As<v8::Int32>()->Value();

  return ToInt64Slow(isolate, value, configuration, exception_state);
}

// Convert a value to a 64-bit unsigned integer. The conversion fails if the
// value cannot be converted to a number or the range violated per WebIDL:
// http://www.w3.org/TR/WebIDL/#es-unsigned-long-long
CORE_EXPORT uint64_t ToUInt64Slow(v8::Isolate*,
                                  v8::Local<v8::Value>,
                                  IntegerConversionConfiguration,
                                  ExceptionState&);
inline uint64_t ToUInt64(v8::Isolate* isolate,
                         v8::Local<v8::Value> value,
                         IntegerConversionConfiguration configuration,
                         ExceptionState& exception_state) {
  // Fast case. The value is a 32-bit unsigned integer.
  if (LIKELY(value->IsUint32()))
    return value.As<v8::Uint32>()->Value();

  if (LIKELY(value->IsInt32() && configuration == kNormalConversion))
    return value.As<v8::Int32>()->Value();

  return ToUInt64Slow(isolate, value, configuration, exception_state);
}

// NaNs and +/-Infinity should be 0, otherwise modulo 2^64.
// Step 8 - 12 of https://heycam.github.io/webidl/#abstract-opdef-converttoint
inline unsigned long long DoubleToInteger(double d) {
  if (std::isnan(d) || std::isinf(d))
    return 0;
  constexpr unsigned long long kMaxULL =
      std::numeric_limits<unsigned long long>::max();

  // -2^{64} < fmod_value < 2^{64}.
  double fmod_value = fmod(trunc(d), kMaxULL + 1.0);
  if (fmod_value >= 0) {
    // 0 <= fmod_value < 2^{64}.
    // 0 <= value < 2^{64}. This cast causes no loss.
    return static_cast<unsigned long long>(fmod_value);
  }
  // -2^{64} < fmod_value < 0.
  // 0 < fmod_value_in_unsigned_long_long < 2^{64}. This cast causes no loss.
  unsigned long long fmod_value_in_unsigned_long_long =
      static_cast<unsigned long long>(-fmod_value);
  // -1 < (kMaxULL - fmod_value_in_unsigned_long_long) < 2^{64} - 1.
  // 0 < value < 2^{64}.
  return kMaxULL - fmod_value_in_unsigned_long_long + 1;
}

// Convert a value to a double precision float, which might fail.
CORE_EXPORT double ToDoubleSlow(v8::Isolate*,
                                v8::Local<v8::Value>,
                                ExceptionState&);
inline double ToDouble(v8::Isolate* isolate,
                       v8::Local<v8::Value> value,
                       ExceptionState& exception_state) {
  if (LIKELY(value->IsNumber()))
    return value.As<v8::Number>()->Value();
  return ToDoubleSlow(isolate, value, exception_state);
}

// Convert a value to a double precision float, throwing on non-finite values.
CORE_EXPORT double ToRestrictedDouble(v8::Isolate*,
                                      v8::Local<v8::Value>,
                                      ExceptionState&);

// Convert a value to a single precision float, which might fail.
inline float ToFloat(v8::Isolate* isolate,
                     v8::Local<v8::Value> value,
                     ExceptionState& exception_state) {
  return static_cast<float>(ToDouble(isolate, value, exception_state));
}

// Convert a value to a single precision float, throwing on non-finite values.
CORE_EXPORT float ToRestrictedFloat(v8::Isolate*,
                                    v8::Local<v8::Value>,
                                    ExceptionState&);

inline double ToCoreDate(v8::Isolate* isolate,
                         v8::Local<v8::Value> object,
                         ExceptionState& exception_state) {
  if (object->IsNull())
    return std::numeric_limits<double>::quiet_NaN();
  if (!object->IsDate()) {
    exception_state.ThrowTypeError("The provided value is not a Date.");
    return 0;
  }
  return object.As<v8::Date>()->ValueOf();
}

// USVString conversion helper.
CORE_EXPORT String ReplaceUnmatchedSurrogates(const String&);

// FIXME: Remove the special casing for XPathNSResolver.
XPathNSResolver* ToXPathNSResolver(ScriptState*, v8::Local<v8::Value>);

template <typename IDLType>
VectorOf<typename NativeValueTraits<IDLType>::ImplType> ToImplArguments(
    const v8::FunctionCallbackInfo<v8::Value>& info,
    int start_index,
    ExceptionState& exception_state) {
  using TraitsType = NativeValueTraits<IDLType>;
  using VectorType = VectorOf<typename TraitsType::ImplType>;

  int length = info.Length();
  VectorType result;
  if (start_index < length) {
    if (static_cast<size_t>(length - start_index) > VectorType::MaxCapacity()) {
      exception_state.ThrowRangeError("Array length exceeds supported limit.");
      return VectorType();
    }
    result.ReserveInitialCapacity(length - start_index);
    for (int i = start_index; i < length; ++i) {
      result.UncheckedAppend(
          TraitsType::NativeValue(info.GetIsolate(), info[i], exception_state));
      if (exception_state.HadException())
        return VectorType();
    }
  }
  return result;
}

// Returns the iterator method for an object, or an empty v8::Local if the
// method is null or undefined.
CORE_EXPORT v8::Local<v8::Function> GetEsIteratorMethod(v8::Isolate*,
                                                        v8::Local<v8::Object>,
                                                        ExceptionState&);

// Gets an iterator for an object, given the iterator method for that object.
CORE_EXPORT v8::Local<v8::Object> GetEsIteratorWithMethod(
    v8::Isolate*,
    v8::Local<v8::Function>,
    v8::Local<v8::Object>,
    ExceptionState&);

// Gets an iterator from an Object.
CORE_EXPORT v8::Local<v8::Object> GetEsIterator(v8::Isolate*,
                                                v8::Local<v8::Object>,
                                                ExceptionState&);

// Validates that the passed object is a sequence type per the WebIDL spec: it
// has a callable @iterator.
// https://heycam.github.io/webidl/#es-sequence
CORE_EXPORT bool HasCallableIteratorSymbol(v8::Isolate*,
                                           v8::Local<v8::Value>,
                                           ExceptionState&);

CORE_EXPORT v8::Isolate* ToIsolate(const ExecutionContext*);
CORE_EXPORT v8::Isolate* ToIsolate(const LocalFrame*);

CORE_EXPORT DOMWindow* ToDOMWindow(v8::Isolate*, v8::Local<v8::Value>);
CORE_EXPORT LocalDOMWindow* ToLocalDOMWindow(v8::Local<v8::Context>);
LocalDOMWindow* EnteredDOMWindow(v8::Isolate*);
CORE_EXPORT LocalDOMWindow* CurrentDOMWindow(v8::Isolate*);
CORE_EXPORT ExecutionContext* ToExecutionContext(v8::Local<v8::Context>);
CORE_EXPORT void RegisterToExecutionContextForModules(ExecutionContext* (
    *to_execution_context_for_modules)(v8::Local<v8::Context>));
CORE_EXPORT ExecutionContext* CurrentExecutionContext(v8::Isolate*);

// Returns a V8 context associated with a ExecutionContext and a
// DOMWrapperWorld.  This method returns an empty context if there is no frame
// or the frame is already detached.
CORE_EXPORT v8::Local<v8::Context> ToV8Context(ExecutionContext*,
                                               DOMWrapperWorld&);
// Returns a V8 context associated with a Frame and a DOMWrapperWorld.
// This method returns an empty context if the frame is already detached.
CORE_EXPORT v8::Local<v8::Context> ToV8Context(LocalFrame*, DOMWrapperWorld&);
// Like toV8Context but also returns the context if the frame is already
// detached.
CORE_EXPORT v8::Local<v8::Context> ToV8ContextEvenIfDetached(LocalFrame*,
                                                             DOMWrapperWorld&);

// These methods can return nullptr if the context associated with the
// ScriptState has already been detached.
CORE_EXPORT ScriptState* ToScriptState(ExecutionContext*, DOMWrapperWorld&);
CORE_EXPORT ScriptState* ToScriptState(LocalFrame*, DOMWrapperWorld&);
// Do not use this method unless you are sure you should use the main world's
// ScriptState
CORE_EXPORT ScriptState* ToScriptStateForMainWorld(LocalFrame*);

// Returns the frame object of the window object associated with
// a context, if the window is currently being displayed in a Frame.
CORE_EXPORT LocalFrame* ToLocalFrameIfNotDetached(v8::Local<v8::Context>);

// If 'storage' is non-null, it must be large enough to copy all bytes in the
// array buffer view into it.  Use allocateFlexibleArrayBufferStorage(v8Value)
// to allocate it using alloca() in the callers stack frame.
CORE_EXPORT void ToFlexibleArrayBufferView(v8::Isolate*,
                                           v8::Local<v8::Value>,
                                           FlexibleArrayBufferView&,
                                           void* storage = nullptr);

CORE_EXPORT bool IsValidEnum(const String& value,
                             const char** valid_values,
                             size_t length,
                             const String& enum_name,
                             ExceptionState&);
CORE_EXPORT bool IsValidEnum(const Vector<String>& values,
                             const char** valid_values,
                             size_t length,
                             const String& enum_name,
                             ExceptionState&);

// Result values for platform object 'deleter' methods,
// http://www.w3.org/TR/WebIDL/#delete
enum DeleteResult { kDeleteSuccess, kDeleteReject, kDeleteUnknownProperty };

CORE_EXPORT v8::Local<v8::Value> FromJSONString(v8::Isolate*,
                                                v8::Local<v8::Context>,
                                                const String& stringified_json,
                                                ExceptionState&);

// Ensure that a typed array value is not backed by a SharedArrayBuffer. If it
// is, an exception will be thrown. The return value will use the NotShared
// wrapper type.
template <typename NotSharedType>
NotSharedType ToNotShared(v8::Isolate* isolate,
                          v8::Local<v8::Value> value,
                          ExceptionState& exception_state) {
  using DOMTypedArray = typename NotSharedType::TypedArrayType;
  DOMTypedArray* dom_typed_array =
      V8TypeOf<DOMTypedArray>::Type::ToImplWithTypeCheck(isolate, value);
  if (dom_typed_array && dom_typed_array->IsShared()) {
    exception_state.ThrowTypeError(
        "The provided ArrayBufferView value must not be shared.");
    return NotSharedType();
  }
  return NotSharedType(dom_typed_array);
}

// Wrap a typed array value in MaybeShared<>, to signify that it may be backed
// by a SharedArrayBuffer.
template <typename MaybeSharedType>
MaybeSharedType ToMaybeShared(v8::Isolate* isolate,
                              v8::Local<v8::Value> value,
                              ExceptionState& exception_state) {
  using DOMTypedArray = typename MaybeSharedType::TypedArrayType;
  DOMTypedArray* dom_typed_array =
      V8TypeOf<DOMTypedArray>::Type::ToImplWithTypeCheck(isolate, value);
  return MaybeSharedType(dom_typed_array);
}

CORE_EXPORT Vector<String> GetOwnPropertyNames(v8::Isolate*,
                                               const v8::Local<v8::Object>&,
                                               ExceptionState&);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_BINDINGS_CORE_V8_V8_BINDING_FOR_CORE_H_
