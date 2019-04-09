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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_FORMS_FORM_DATA_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_FORMS_FORM_DATA_H_

#include "third_party/blink/renderer/bindings/core/v8/file_or_usv_string.h"
#include "third_party/blink/renderer/bindings/core/v8/iterable.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/network/encoded_form_data.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/text/text_encoding.h"

namespace blink {

class Blob;
class HTMLFormElement;
class ScriptState;

// Typedef from form_data.idl:
typedef FileOrUSVString FormDataEntryValue;

class CORE_EXPORT FormData final
    : public ScriptWrappable,
      public PairIterable<String, FormDataEntryValue> {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static FormData* Create() { return MakeGarbageCollected<FormData>(); }
  static FormData* Create(ExceptionState& exception_state) {
    return MakeGarbageCollected<FormData>();
  }
  static FormData* Create(HTMLFormElement* form,
                          ExceptionState& exception_state);

  static FormData* Create(const WTF::TextEncoding& encoding) {
    return MakeGarbageCollected<FormData>(encoding);
  }

  explicit FormData(const WTF::TextEncoding&);
  FormData();
  void Trace(blink::Visitor*) override;

  // FormData IDL interface.
  void append(const String& name, const String& value);
  void append(ScriptState*,
              const String& name,
              Blob*,
              const String& filename = String());
  void deleteEntry(const String& name);
  void get(const String& name, FormDataEntryValue& result);
  HeapVector<FormDataEntryValue> getAll(const String& name);
  bool has(const String& name);
  void set(const String& name, const String& value);
  void set(const String& name, Blob*, const String& filename = String());

  // Internal functions.

  const WTF::TextEncoding& Encoding() const { return encoding_; }
  CString Encode(const String& key) const;
  class Entry;
  const HeapVector<Member<const Entry>>& Entries() const { return entries_; }
  size_t size() const { return entries_.size(); }
  void append(const String& name, Blob*, const String& filename = String());
  void AppendFromElement(const String& name, int value);
  void AppendFromElement(const String& name, File* file);
  void AppendFromElement(const String& name, const String& value);

  // This flag is true if this FormData is created with a <form>, and its
  // associated elements contain a non-empty password field.
  bool ContainsPasswordData() const { return contains_password_data_; }
  void SetContainsPasswordData(bool flag) { contains_password_data_ = flag; }

  scoped_refptr<EncodedFormData> EncodeFormData(
      EncodedFormData::EncodingType = EncodedFormData::kFormURLEncoded);
  scoped_refptr<EncodedFormData> EncodeMultiPartFormData();

 private:
  void SetEntry(const Entry*);
  IterationSource* StartIteration(ScriptState*, ExceptionState&) override;

  WTF::TextEncoding encoding_;
  // Entry pointers in entries_ never be nullptr.
  HeapVector<Member<const Entry>> entries_;
  bool contains_password_data_ = false;
};

// Represents entry, which is a pair of a name and a value.
// https://xhr.spec.whatwg.org/#concept-formdata-entry
// Entry objects are immutable.
class FormData::Entry : public GarbageCollectedFinalized<FormData::Entry> {
 public:
  Entry(const String& name, const String& value);
  Entry(const String& name, Blob* blob, const String& filename);
  void Trace(blink::Visitor*);

  bool IsString() const { return !blob_; }
  bool isFile() const { return blob_; }
  const String& name() const { return name_; }
  const String& Value() const { return value_; }
  Blob* GetBlob() const { return blob_.Get(); }
  File* GetFile() const;
  const String& Filename() const { return filename_; }

 private:
  const String name_;
  const String value_;
  const Member<Blob> blob_;
  const String filename_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_FORMS_FORM_DATA_H_
