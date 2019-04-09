/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_V8_BINDING_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_V8_BINDING_H_

#include "third_party/blink/renderer/platform/bindings/dom_data_store.h"
#include "third_party/blink/renderer/platform/bindings/dom_wrapper_world.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/bindings/string_resource.h"
#include "third_party/blink/renderer/platform/bindings/v8_binding_macros.h"
#include "third_party/blink/renderer/platform/bindings/v8_per_isolate_data.h"
#include "third_party/blink/renderer/platform/bindings/v8_value_cache.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/text/atomic_string.h"
#include "v8/include/v8.h"

namespace blink {

// This file contains bindings helper functions that do not have dependencies
// to core/ or bindings/core. For core-specific helper functions, see
// bindings/core/v8/V8BindingForCore.h.

template <typename T>
struct V8TypeOf {
  STATIC_ONLY(V8TypeOf);
  // |Type| provides C++ -> V8 type conversion for DOM wrappers.
  // The Blink binding code generator will generate specialized version of
  // V8TypeOf for each wrapper class.
  typedef void Type;
};

template <typename CallbackInfo, typename S>
inline void V8SetReturnValue(const CallbackInfo& info,
                             const v8::Persistent<S>& handle) {
  info.GetReturnValue().Set(handle);
}

template <typename CallbackInfo, typename S>
inline void V8SetReturnValue(const CallbackInfo& info,
                             const v8::Local<S> handle) {
  info.GetReturnValue().Set(handle);
}

template <typename CallbackInfo, typename S>
inline void V8SetReturnValue(const CallbackInfo& info,
                             v8::MaybeLocal<S> maybe) {
  if (LIKELY(!maybe.IsEmpty()))
    info.GetReturnValue().Set(maybe.ToLocalChecked());
}

template <typename CallbackInfo>
inline void V8SetReturnValue(const CallbackInfo& info, bool value) {
  info.GetReturnValue().Set(value);
}

template <typename CallbackInfo>
inline void V8SetReturnValue(const CallbackInfo& info, double value) {
  info.GetReturnValue().Set(value);
}

template <typename CallbackInfo>
inline void V8SetReturnValue(const CallbackInfo& info, int32_t value) {
  info.GetReturnValue().Set(value);
}

template <typename CallbackInfo>
inline void V8SetReturnValue(const CallbackInfo& info, uint32_t value) {
  info.GetReturnValue().Set(value);
}

template <typename CallbackInfo>
inline void V8SetReturnValueBool(const CallbackInfo& info, bool v) {
  info.GetReturnValue().Set(v);
}

template <typename CallbackInfo>
inline void V8SetReturnValueInt(const CallbackInfo& info, int v) {
  info.GetReturnValue().Set(v);
}

template <typename CallbackInfo>
inline void V8SetReturnValueUnsigned(const CallbackInfo& info, unsigned v) {
  info.GetReturnValue().Set(v);
}

template <typename CallbackInfo>
inline void V8SetReturnValueNull(const CallbackInfo& info) {
  info.GetReturnValue().SetNull();
}

template <typename CallbackInfo>
inline void V8SetReturnValueUndefined(const CallbackInfo& info) {
  info.GetReturnValue().SetUndefined();
}

template <typename CallbackInfo>
inline void V8SetReturnValueEmptyString(const CallbackInfo& info) {
  info.GetReturnValue().SetEmptyString();
}

template <typename CallbackInfo>
inline void V8SetReturnValueString(const CallbackInfo& info,
                                   const String& string,
                                   v8::Isolate* isolate) {
  if (string.IsNull()) {
    V8SetReturnValueEmptyString(info);
    return;
  }
  V8PerIsolateData::From(isolate)->GetStringCache()->SetReturnValueFromString(
      info.GetReturnValue(), string.Impl());
}

template <typename CallbackInfo>
inline void V8SetReturnValueStringOrNull(const CallbackInfo& info,
                                         const String& string,
                                         v8::Isolate* isolate) {
  if (string.IsNull()) {
    V8SetReturnValueNull(info);
    return;
  }
  V8PerIsolateData::From(isolate)->GetStringCache()->SetReturnValueFromString(
      info.GetReturnValue(), string.Impl());
}

template <typename CallbackInfo>
inline void V8SetReturnValue(const CallbackInfo& callback_info,
                             ScriptWrappable* impl,
                             v8::Local<v8::Object> creation_context) {
  if (UNLIKELY(!impl)) {
    V8SetReturnValueNull(callback_info);
    return;
  }
  if (DOMDataStore::SetReturnValue(callback_info.GetReturnValue(), impl))
    return;
  v8::Local<v8::Object> wrapper =
      impl->Wrap(callback_info.GetIsolate(), creation_context);
  V8SetReturnValue(callback_info, wrapper);
}

template <typename CallbackInfo>
inline void V8SetReturnValue(const CallbackInfo& callback_info,
                             ScriptWrappable* impl) {
  V8SetReturnValue(callback_info, impl, callback_info.Holder());
}

template <typename CallbackInfo>
inline void V8SetReturnValueForMainWorld(const CallbackInfo& callback_info,
                                         ScriptWrappable* impl) {
  DCHECK(DOMWrapperWorld::Current(callback_info.GetIsolate()).IsMainWorld());
  if (UNLIKELY(!impl)) {
    V8SetReturnValueNull(callback_info);
    return;
  }
  if (DOMDataStore::SetReturnValueForMainWorld(callback_info.GetReturnValue(),
                                               impl))
    return;
  v8::Local<v8::Object> wrapper =
      impl->Wrap(callback_info.GetIsolate(), callback_info.Holder());
  V8SetReturnValue(callback_info, wrapper);
}

template <typename CallbackInfo>
inline void V8SetReturnValueFast(const CallbackInfo& callback_info,
                                 ScriptWrappable* impl,
                                 const ScriptWrappable* wrappable) {
  if (UNLIKELY(!impl)) {
    V8SetReturnValueNull(callback_info);
    return;
  }
  if (DOMDataStore::SetReturnValueFast(callback_info.GetReturnValue(), impl,
                                       callback_info.Holder(), wrappable))
    return;
  v8::Local<v8::Object> wrapper =
      impl->Wrap(callback_info.GetIsolate(), callback_info.Holder());
  V8SetReturnValue(callback_info, wrapper);
}

template <typename CallbackInfo, typename T>
inline void V8SetReturnValueFast(const CallbackInfo& callback_info,
                                 const v8::Local<T> handle,
                                 const ScriptWrappable*) {
  V8SetReturnValue(callback_info, handle);
}

// Convert v8::String to a WTF::String. If the V8 string is not already
// an external string then it is transformed into an external string at this
// point to avoid repeated conversions.
inline String ToCoreString(v8::Local<v8::String> value) {
  return ToBlinkString<String>(value, kExternalize);
}

inline String ToCoreStringWithNullCheck(v8::Local<v8::String> value) {
  if (value.IsEmpty() || value->IsNull())
    return String();
  return ToCoreString(value);
}

inline String ToCoreStringWithUndefinedOrNullCheck(
    v8::Local<v8::String> value) {
  if (value.IsEmpty())
    return String();
  return ToCoreString(value);
}

inline AtomicString ToCoreAtomicString(v8::Local<v8::String> value) {
  return ToBlinkString<AtomicString>(value, kExternalize);
}

// This method will return a null String if the v8::Value does not contain a
// v8::String.  It will not call ToString() on the v8::Value. If you want
// ToString() to be called, please use the TONATIVE_FOR_V8STRINGRESOURCE_*()
// macros instead.
inline String ToCoreStringWithUndefinedOrNullCheck(v8::Local<v8::Value> value) {
  if (value.IsEmpty() || !value->IsString())
    return String();
  return ToCoreString(value.As<v8::String>());
}

// Convert a string to a V8 string.

inline v8::Local<v8::String> V8String(v8::Isolate* isolate,
                                      const StringView& string) {
  DCHECK(isolate);
  if (string.IsNull())
    return v8::String::Empty(isolate);
  if (StringImpl* impl = string.SharedImpl()) {
    return V8PerIsolateData::From(isolate)->GetStringCache()->V8ExternalString(
        isolate, impl);
  }
  if (string.Is8Bit()) {
    return v8::String::NewFromOneByte(
               isolate, reinterpret_cast<const uint8_t*>(string.Characters8()),
               v8::NewStringType::kNormal, static_cast<int>(string.length()))
        .ToLocalChecked();
  }
  return v8::String::NewFromTwoByte(
             isolate, reinterpret_cast<const uint16_t*>(string.Characters16()),
             v8::NewStringType::kNormal, static_cast<int>(string.length()))
      .ToLocalChecked();
}

// As above, for string literals. The compiler doesn't optimize away the is8Bit
// and sharedImpl checks for string literals in the StringView version.
inline v8::Local<v8::String> V8String(v8::Isolate* isolate,
                                      const char* string) {
  DCHECK(isolate);
  if (!string || string[0] == '\0')
    return v8::String::Empty(isolate);
  return v8::String::NewFromOneByte(isolate,
                                    reinterpret_cast<const uint8_t*>(string),
                                    v8::NewStringType::kNormal, strlen(string))
      .ToLocalChecked();
}

inline v8::Local<v8::Value> V8StringOrNull(v8::Isolate* isolate,
                                           const AtomicString& string) {
  if (string.IsNull())
    return v8::Null(isolate);
  return V8PerIsolateData::From(isolate)->GetStringCache()->V8ExternalString(
      isolate, string.Impl());
}

inline v8::Local<v8::String> V8String(v8::Isolate* isolate,
                                      const ParkableString& string) {
  if (string.IsNull())
    return v8::String::Empty(isolate);
  return V8PerIsolateData::From(isolate)->GetStringCache()->V8ExternalString(
      isolate, string);
}

inline v8::Local<v8::String> V8AtomicString(v8::Isolate* isolate,
                                            const StringView& string) {
  DCHECK(isolate);
  if (string.Is8Bit()) {
    return v8::String::NewFromOneByte(
               isolate, reinterpret_cast<const uint8_t*>(string.Characters8()),
               v8::NewStringType::kInternalized,
               static_cast<int>(string.length()))
        .ToLocalChecked();
  }
  return v8::String::NewFromTwoByte(
             isolate, reinterpret_cast<const uint16_t*>(string.Characters16()),
             v8::NewStringType::kInternalized,
             static_cast<int>(string.length()))
      .ToLocalChecked();
}

// As above, for string literals. The compiler doesn't optimize away the is8Bit
// check for string literals in the StringView version.
inline v8::Local<v8::String> V8AtomicString(v8::Isolate* isolate,
                                            const char* string) {
  DCHECK(isolate);
  if (!string || string[0] == '\0')
    return v8::String::Empty(isolate);
  return v8::String::NewFromOneByte(
             isolate, reinterpret_cast<const uint8_t*>(string),
             v8::NewStringType::kInternalized, strlen(string))
      .ToLocalChecked();
}

inline bool IsUndefinedOrNull(v8::Local<v8::Value> value) {
  return value.IsEmpty() || value->IsNullOrUndefined();
}
PLATFORM_EXPORT v8::Local<v8::Function> GetBoundFunction(
    v8::Local<v8::Function>);

// FIXME: This will be soon embedded in the generated code.
template <typename Collection>
static void IndexedPropertyEnumerator(
    const v8::PropertyCallbackInfo<v8::Array>& info) {
  Collection* collection =
      ToScriptWrappable(info.Holder())->ToImpl<Collection>();
  int length = collection->length();
  v8::Local<v8::Array> properties = v8::Array::New(info.GetIsolate(), length);
  v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();
  for (int i = 0; i < length; ++i) {
    v8::Local<v8::Integer> integer = v8::Integer::New(info.GetIsolate(), i);
    bool created;
    if (!properties->CreateDataProperty(context, i, integer).To(&created))
      return;
    DCHECK(created);
  }
  V8SetReturnValue(info, properties);
}

// Freeze a V8 object. The type of the first parameter and the return value is
// intentionally v8::Value so that this function can wrap ToV8().
// If the argument isn't an object, this will crash.
PLATFORM_EXPORT v8::Local<v8::Value> FreezeV8Object(v8::Local<v8::Value>,
                                                    v8::Isolate*);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_V8_BINDING_H_
