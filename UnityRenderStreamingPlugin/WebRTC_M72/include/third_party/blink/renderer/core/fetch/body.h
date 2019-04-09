// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FETCH_BODY_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FETCH_BODY_H_

#include "third_party/blink/renderer/bindings/core/v8/active_script_wrappable.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise.h"
#include "third_party/blink/renderer/bindings/core/v8/script_value.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/context_lifecycle_observer.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class BodyStreamBuffer;
class ExceptionState;
class ExecutionContext;
class ReadableStream;
class ScriptState;

// This class represents Body mix-in defined in the fetch spec
// https://fetch.spec.whatwg.org/#body-mixin.
//
// Note: This class has body stream and its predicate whereas in the current
// spec only Response has it and Request has a byte stream defined in the
// Encoding spec. The spec should be fixed shortly to be aligned with this
// implementation.
class CORE_EXPORT Body : public ScriptWrappable,
                         public ActiveScriptWrappable<Body>,
                         public ContextClient {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(Body);

 public:
  enum class BodyUsed { kUsed, kUnused, kBroken };
  enum class BodyLocked { kLocked, kUnlocked, kBroken };

  explicit Body(ExecutionContext*);

  ScriptPromise arrayBuffer(ScriptState*, ExceptionState&);
  ScriptPromise blob(ScriptState*, ExceptionState&);
  ScriptPromise formData(ScriptState*, ExceptionState&);
  ScriptPromise json(ScriptState*, ExceptionState&);
  ScriptPromise text(ScriptState*, ExceptionState&);
  ReadableStream* body();
  virtual BodyStreamBuffer* BodyBuffer() = 0;
  virtual const BodyStreamBuffer* BodyBuffer() const = 0;

  // This should only be called from the generated bindings. All other code
  // should use IsBodyUsed() instead.
  bool bodyUsed(ExceptionState& exception_state) {
    return IsBodyUsed(exception_state) == BodyUsed::kUsed;
  }

  // Returns kUsed, kUnused or kBroken. kBroken implies there is an exception
  // pending and the caller should return to JavaScript immediately.
  virtual BodyUsed IsBodyUsed(ExceptionState&);

  // Returns kLocked, kUnlocked or kBroken. kBroken implies there is an
  // exception pending and the caller should return to JavaScript immediately.
  BodyLocked IsBodyLocked(ExceptionState&);

  // ScriptWrappable override.
  bool HasPendingActivity() const override;

  void Trace(blink::Visitor* visitor) override {
    ScriptWrappable::Trace(visitor);
    ContextClient::Trace(visitor);
  }

 protected:
  // A version of IsBodyUsed() which catches exceptions and returns
  // false. Should never be used outside DCHECK().
  virtual bool IsBodyUsedForDCheck(ExceptionState& exception_state);

 private:
  // TODO(e_hakkinen): Fix |MimeType()| to always contain parameters and
  // remove |ContentType()|.
  virtual String ContentType() const = 0;
  virtual String MimeType() const = 0;

  // Body consumption algorithms will reject with a TypeError in a number of
  // error conditions. This method wraps those up into one call which throws
  // an exception if consumption cannot proceed. The caller must check
  // |exception_state| on return.
  void RejectInvalidConsumption(ScriptState*, ExceptionState& exception_state);
  DISALLOW_COPY_AND_ASSIGN(Body);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FETCH_BODY_H_
