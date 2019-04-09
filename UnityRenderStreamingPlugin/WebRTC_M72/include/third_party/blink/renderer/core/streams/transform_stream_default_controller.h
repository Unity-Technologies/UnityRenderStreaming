// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_STREAMS_TRANSFORM_STREAM_DEFAULT_CONTROLLER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_STREAMS_TRANSFORM_STREAM_DEFAULT_CONTROLLER_H_

#include "base/macros.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/bindings/exception_state.h"
#include "third_party/blink/renderer/platform/bindings/script_state.h"
#include "third_party/blink/renderer/platform/heap/member.h"
#include "v8/include/v8.h"

namespace blink {

// Thin wrapper for the JavaScript TransformStreamDefaultController object. The
// API mimics the JavaScript API
// https://streams.spec.whatwg.org/#ts-default-controller-class but unneeded
// parts have not been implemented.
class CORE_EXPORT TransformStreamDefaultController final {
  STACK_ALLOCATED();

 public:
  TransformStreamDefaultController(ScriptState*,
                                   v8::Local<v8::Value> controller);

  void Enqueue(v8::Local<v8::Value> chunk, ExceptionState&);

 private:
  Member<ScriptState> script_state_;
  v8::Local<v8::Value> controller_;

  DISALLOW_COPY_AND_ASSIGN(TransformStreamDefaultController);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_STREAMS_TRANSFORM_STREAM_DEFAULT_CONTROLLER_H_
