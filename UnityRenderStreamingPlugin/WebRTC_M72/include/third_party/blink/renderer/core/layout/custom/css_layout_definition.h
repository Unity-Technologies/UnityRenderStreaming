// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_CUSTOM_CSS_LAYOUT_DEFINITION_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_CUSTOM_CSS_LAYOUT_DEFINITION_H_

#include "third_party/blink/renderer/core/css/css_property_names.h"
#include "third_party/blink/renderer/core/css/cssom/css_style_value.h"
#include "third_party/blink/renderer/platform/bindings/name_client.h"
#include "third_party/blink/renderer/platform/bindings/scoped_persistent.h"
#include "third_party/blink/renderer/platform/bindings/trace_wrapper_member.h"
#include "third_party/blink/renderer/platform/bindings/trace_wrapper_v8_reference.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "v8/include/v8.h"

namespace blink {

class FragmentResultOptions;
class LayoutCustom;
class ScriptState;
class SerializedScriptValue;

// Represents a javascript class registered on the LayoutWorkletGlobalScope by
// the author.
// https://drafts.css-houdini.org/css-layout-api/#layout-definition
class CSSLayoutDefinition final
    : public GarbageCollectedFinalized<CSSLayoutDefinition>,
      public NameClient {
 public:
  CSSLayoutDefinition(
      ScriptState*,
      v8::Local<v8::Function> constructor,
      v8::Local<v8::Function> intrinsic_sizes,
      v8::Local<v8::Function> layout,
      const Vector<CSSPropertyID>& native_invalidation_properties,
      const Vector<AtomicString>& custom_invalidation_properties,
      const Vector<CSSPropertyID>& child_native_invalidation_properties,
      const Vector<AtomicString>& child_custom_invalidation_properties);
  virtual ~CSSLayoutDefinition();

  // This class represents an instance of the layout class defined by the
  // CSSLayoutDefinition.
  class Instance final : public GarbageCollectedFinalized<Instance> {
   public:
    Instance(CSSLayoutDefinition*, v8::Local<v8::Value> instance);

    // Runs the web developer defined layout, returns true if everything
    // succeeded. It populates the FragmentResultOptions dictionary, and
    // fragment_result_data.
    bool Layout(const LayoutCustom&,
                FragmentResultOptions*,
                scoped_refptr<SerializedScriptValue>* fragment_result_data);

    void Trace(blink::Visitor*);

   private:
    void ReportException(ExceptionState*);

    Member<CSSLayoutDefinition> definition_;
    ScopedPersistent<v8::Value> instance_;
  };

  // Creates an instance of the web developer defined class. May return a
  // nullptr if constructing the instance failed.
  Instance* CreateInstance();

  const Vector<CSSPropertyID>& NativeInvalidationProperties() const {
    return native_invalidation_properties_;
  }
  const Vector<AtomicString>& CustomInvalidationProperties() const {
    return custom_invalidation_properties_;
  }
  const Vector<CSSPropertyID>& ChildNativeInvalidationProperties() const {
    return child_native_invalidation_properties_;
  }
  const Vector<AtomicString>& ChildCustomInvalidationProperties() const {
    return child_custom_invalidation_properties_;
  }

  ScriptState* GetScriptState() const { return script_state_; }

  v8::Local<v8::Function> LayoutFunctionForTesting(v8::Isolate* isolate) {
    return layout_.NewLocal(isolate);
  }

  virtual void Trace(blink::Visitor* visitor);

  const char* NameInHeapSnapshot() const override {
    return "CSSLayoutDefinition";
  }

 private:
  Member<ScriptState> script_state_;

  // This object keeps the class instances, constructor function, intrinsic
  // sizes function, and layout function alive. It participates in wrapper
  // tracing as it holds onto V8 wrappers.
  TraceWrapperV8Reference<v8::Function> constructor_;
  TraceWrapperV8Reference<v8::Function> intrinsic_sizes_;
  TraceWrapperV8Reference<v8::Function> layout_;

  // If a constructor call ever fails, we'll refuse to create any more
  // instances of the web developer provided class.
  bool constructor_has_failed_;

  Vector<CSSPropertyID> native_invalidation_properties_;
  Vector<AtomicString> custom_invalidation_properties_;
  Vector<CSSPropertyID> child_native_invalidation_properties_;
  Vector<AtomicString> child_custom_invalidation_properties_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_CUSTOM_CSS_LAYOUT_DEFINITION_H_
