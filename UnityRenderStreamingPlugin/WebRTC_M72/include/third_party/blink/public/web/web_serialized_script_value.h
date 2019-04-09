/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_SERIALIZED_SCRIPT_VALUE_H_
#define THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_SERIALIZED_SCRIPT_VALUE_H_

#include "third_party/blink/public/platform/web_common.h"
#include "third_party/blink/public/platform/web_private_ptr.h"

namespace v8 {
class Isolate;
class Value;
template <class T>
class Local;
}

namespace blink {

class SerializedScriptValue;
class WebString;

// FIXME: Should this class be in platform?
class WebSerializedScriptValue {
 public:
  ~WebSerializedScriptValue() { Reset(); }

  WebSerializedScriptValue() = default;
  WebSerializedScriptValue(const WebSerializedScriptValue& d) { Assign(d); }
  WebSerializedScriptValue& operator=(const WebSerializedScriptValue& d) {
    Assign(d);
    return *this;
  }

  // Creates a serialized script value from its wire format data.
  BLINK_EXPORT static WebSerializedScriptValue FromString(const WebString&);

  BLINK_EXPORT static WebSerializedScriptValue Serialize(v8::Isolate*,
                                                         v8::Local<v8::Value>);

  // Create a WebSerializedScriptValue that represents a serialization error.
  BLINK_EXPORT static WebSerializedScriptValue CreateInvalid();

  BLINK_EXPORT void Reset();
  BLINK_EXPORT void Assign(const WebSerializedScriptValue&);

  bool IsNull() const { return private_.IsNull(); }

  // Returns a string representation of the WebSerializedScriptValue.
  BLINK_EXPORT WebString ToString() const;

  // Convert the serialized value to a parsed v8 value.
  BLINK_EXPORT v8::Local<v8::Value> Deserialize(v8::Isolate*);

#if INSIDE_BLINK
  BLINK_EXPORT WebSerializedScriptValue(scoped_refptr<SerializedScriptValue>);
  BLINK_EXPORT WebSerializedScriptValue& operator=(
      scoped_refptr<SerializedScriptValue>);
  BLINK_EXPORT operator scoped_refptr<SerializedScriptValue>() const;
#endif

 private:
  WebPrivatePtr<SerializedScriptValue> private_;
};

}  // namespace blink

#endif
