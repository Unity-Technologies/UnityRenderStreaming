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
#ifndef THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_DOM_FILE_SYSTEM_H_
#define THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_DOM_FILE_SYSTEM_H_

#include "third_party/blink/public/platform/web_common.h"
#include "third_party/blink/public/platform/web_file_system_type.h"
#include "third_party/blink/public/platform/web_private_ptr.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/platform/web_url.h"
#include "third_party/blink/public/web/web_frame.h"

namespace v8 {
class Isolate;
class Object;
class Value;
template <class T>
class Local;
}

namespace blink {

class DOMFileSystem;

class WebDOMFileSystem {
 public:
  enum SerializableType {
    kSerializableTypeSerializable,
    kSerializableTypeNotSerializable,
  };
  enum EntryType {
    kEntryTypeFile,
    kEntryTypeDirectory,
  };

  ~WebDOMFileSystem() { Reset(); }

  WebDOMFileSystem() = default;
  WebDOMFileSystem(const WebDOMFileSystem& d) { Assign(d); }
  WebDOMFileSystem& operator=(const WebDOMFileSystem& d) {
    Assign(d);
    return *this;
  }

  BLINK_EXPORT static WebDOMFileSystem FromV8Value(v8::Local<v8::Value>);
  // Create file system URL from the given entry.
  BLINK_EXPORT static WebURL CreateFileSystemURL(v8::Local<v8::Value> entry);

  // FIXME: Deprecate the last argument when all filesystems become
  // serializable.
  BLINK_EXPORT static WebDOMFileSystem Create(
      WebLocalFrame*,
      WebFileSystemType,
      const WebString& name,
      const WebURL& root_url,
      SerializableType = kSerializableTypeNotSerializable);

  BLINK_EXPORT void Reset();
  BLINK_EXPORT void Assign(const WebDOMFileSystem&);

  BLINK_EXPORT WebString GetName() const;
  BLINK_EXPORT WebFileSystemType GetType() const;
  BLINK_EXPORT WebURL RootURL() const;

  BLINK_EXPORT v8::Local<v8::Value> ToV8Value(
      v8::Local<v8::Object> creation_context,
      v8::Isolate*);
  BLINK_EXPORT v8::Local<v8::Value> CreateV8Entry(
      const WebString& path,
      EntryType,
      v8::Local<v8::Object> creation_context,
      v8::Isolate*);

  bool IsNull() const { return private_.IsNull(); }

#if INSIDE_BLINK
  WebDOMFileSystem(DOMFileSystem*);
  WebDOMFileSystem& operator=(DOMFileSystem*);
#endif

 private:
  WebPrivatePtr<DOMFileSystem> private_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_DOM_FILE_SYSTEM_H_
