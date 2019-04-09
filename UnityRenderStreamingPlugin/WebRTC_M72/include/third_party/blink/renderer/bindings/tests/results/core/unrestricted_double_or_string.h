// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file has been auto-generated from the Jinja2 template
// third_party/blink/renderer/bindings/templates/union_container.h.tmpl
// by the script code_generator_v8.py.
// DO NOT MODIFY!

// clang-format off
#ifndef THIRD_PARTY_BLINK_RENDERER_BINDINGS_TESTS_RESULTS_CORE_UNRESTRICTED_DOUBLE_OR_STRING_H_
#define THIRD_PARTY_BLINK_RENDERER_BINDINGS_TESTS_RESULTS_CORE_UNRESTRICTED_DOUBLE_OR_STRING_H_

#include "base/optional.h"
#include "third_party/blink/renderer/bindings/core/v8/dictionary.h"
#include "third_party/blink/renderer/bindings/core/v8/native_value_traits.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_binding_for_core.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/bindings/exception_state.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class CORE_EXPORT UnrestrictedDoubleOrString final {
  DISALLOW_NEW();
 public:
  UnrestrictedDoubleOrString();
  bool IsNull() const { return type_ == SpecificType::kNone; }

  bool IsString() const { return type_ == SpecificType::kString; }
  const String& GetAsString() const;
  void SetString(const String&);
  static UnrestrictedDoubleOrString FromString(const String&);

  bool IsUnrestrictedDouble() const { return type_ == SpecificType::kUnrestrictedDouble; }
  double GetAsUnrestrictedDouble() const;
  void SetUnrestrictedDouble(double);
  static UnrestrictedDoubleOrString FromUnrestrictedDouble(double);

  UnrestrictedDoubleOrString(const UnrestrictedDoubleOrString&);
  ~UnrestrictedDoubleOrString();
  UnrestrictedDoubleOrString& operator=(const UnrestrictedDoubleOrString&);
  void Trace(blink::Visitor*);

 private:
  enum class SpecificType {
    kNone,
    kString,
    kUnrestrictedDouble,
  };
  SpecificType type_;

  String string_;
  double unrestricted_double_;

  friend CORE_EXPORT v8::Local<v8::Value> ToV8(const UnrestrictedDoubleOrString&, v8::Local<v8::Object>, v8::Isolate*);
};

class V8UnrestrictedDoubleOrString final {
 public:
  CORE_EXPORT static void ToImpl(v8::Isolate*, v8::Local<v8::Value>, UnrestrictedDoubleOrString&, UnionTypeConversionMode, ExceptionState&);
};

CORE_EXPORT v8::Local<v8::Value> ToV8(const UnrestrictedDoubleOrString&, v8::Local<v8::Object>, v8::Isolate*);

template <class CallbackInfo>
inline void V8SetReturnValue(const CallbackInfo& callbackInfo, UnrestrictedDoubleOrString& impl) {
  V8SetReturnValue(callbackInfo, ToV8(impl, callbackInfo.Holder(), callbackInfo.GetIsolate()));
}

template <class CallbackInfo>
inline void V8SetReturnValue(const CallbackInfo& callbackInfo, UnrestrictedDoubleOrString& impl, v8::Local<v8::Object> creationContext) {
  V8SetReturnValue(callbackInfo, ToV8(impl, creationContext, callbackInfo.GetIsolate()));
}

template <>
struct NativeValueTraits<UnrestrictedDoubleOrString> : public NativeValueTraitsBase<UnrestrictedDoubleOrString> {
  CORE_EXPORT static UnrestrictedDoubleOrString NativeValue(v8::Isolate*, v8::Local<v8::Value>, ExceptionState&);
  CORE_EXPORT static UnrestrictedDoubleOrString NullValue() { return UnrestrictedDoubleOrString(); }
};

template <>
struct V8TypeOf<UnrestrictedDoubleOrString> {
  typedef V8UnrestrictedDoubleOrString Type;
};

}  // namespace blink

// We need to set canInitializeWithMemset=true because HeapVector supports
// items that can initialize with memset or have a vtable. It is safe to
// set canInitializeWithMemset=true for a union type object in practice.
// See https://codereview.chromium.org/1118993002/#msg5 for more details.
WTF_ALLOW_MOVE_AND_INIT_WITH_MEM_FUNCTIONS(blink::UnrestrictedDoubleOrString);

#endif  // THIRD_PARTY_BLINK_RENDERER_BINDINGS_TESTS_RESULTS_CORE_UNRESTRICTED_DOUBLE_OR_STRING_H_
