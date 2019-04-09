// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_BINDINGS_MODULES_V8_WEBGL_ANY_H_
#define THIRD_PARTY_BLINK_RENDERER_BINDINGS_MODULES_V8_WEBGL_ANY_H_

#include "base/memory/scoped_refptr.h"
#include "third_party/blink/renderer/bindings/core/v8/script_value.h"
#include "third_party/blink/renderer/core/typed_arrays/dom_typed_array.h"
#include "third_party/blink/renderer/modules/webgl/webgl_object.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

namespace blink {

ScriptValue WebGLAny(ScriptState*, bool value);
ScriptValue WebGLAny(ScriptState*, const bool* value, uint32_t);
ScriptValue WebGLAny(ScriptState*, const Vector<bool>& value);
ScriptValue WebGLAny(ScriptState*, const Vector<unsigned>& value);
ScriptValue WebGLAny(ScriptState*, const Vector<int>& value);
ScriptValue WebGLAny(ScriptState*, int value);
ScriptValue WebGLAny(ScriptState*, unsigned value);
ScriptValue WebGLAny(ScriptState*, int64_t value);
ScriptValue WebGLAny(ScriptState*, uint64_t value);
ScriptValue WebGLAny(ScriptState*, float value);
ScriptValue WebGLAny(ScriptState*, String value);
ScriptValue WebGLAny(ScriptState*, WebGLObject* value);
ScriptValue WebGLAny(ScriptState*, DOMFloat32Array* value);
ScriptValue WebGLAny(ScriptState*, DOMInt32Array* value);
ScriptValue WebGLAny(ScriptState*, DOMUint8Array* value);
ScriptValue WebGLAny(ScriptState*, DOMUint32Array* value);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_BINDINGS_MODULES_V8_WEBGL_ANY_H_
