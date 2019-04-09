// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_BINDINGS_CORE_V8_SERIALIZATION_V8_SCRIPT_VALUE_SERIALIZER_H_
#define THIRD_PARTY_BLINK_RENDERER_BINDINGS_CORE_V8_SERIALIZATION_V8_SCRIPT_VALUE_SERIALIZER_H_

#include "base/memory/scoped_refptr.h"
#include "third_party/blink/renderer/bindings/core/v8/serialization/serialization_tag.h"
#include "third_party/blink/renderer/bindings/core/v8/serialization/serialized_color_params.h"
#include "third_party/blink/renderer/bindings/core/v8/serialization/serialized_script_value.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/bindings/exception_state.h"
#include "third_party/blink/renderer/platform/bindings/script_state.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/noncopyable.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"
#include "v8/include/v8.h"

namespace blink {

class File;
class Transferables;

// Serializes V8 values according to the HTML structured clone algorithm:
// https://html.spec.whatwg.org/multipage/infrastructure.html#structured-clone
//
// Supports only basic JavaScript objects and core DOM types. Support for
// modules types is implemented in a subclass.
//
// A serializer cannot be used multiple times; it is expected that its serialize
// method will be invoked exactly once.
class CORE_EXPORT V8ScriptValueSerializer
    : public v8::ValueSerializer::Delegate {
  STACK_ALLOCATED();
  WTF_MAKE_NONCOPYABLE(V8ScriptValueSerializer);

 public:
  using Options = SerializedScriptValue::SerializeOptions;
  explicit V8ScriptValueSerializer(ScriptState*, const Options& = Options());

  scoped_refptr<SerializedScriptValue> Serialize(v8::Local<v8::Value>,
                                                 ExceptionState&);

 protected:
  // Returns true if the DOM object was successfully written.
  // If false is returned and no more specific exception is thrown, a generic
  // DataCloneError message will be used.
  virtual bool WriteDOMObject(ScriptWrappable*, ExceptionState&);

  void WriteTag(SerializationTag tag) {
    uint8_t tag_byte = tag;
    serializer_.WriteRawBytes(&tag_byte, 1);
  }
  void WriteUint32(uint32_t value) { serializer_.WriteUint32(value); }
  void WriteUint64(uint64_t value) { serializer_.WriteUint64(value); }
  void WriteDouble(double value) { serializer_.WriteDouble(value); }
  void WriteRawBytes(const void* data, size_t size) {
    serializer_.WriteRawBytes(data, size);
  }
  void WriteUTF8String(const String&);

  template <typename E>
  void WriteUint32Enum(E value) {
    static_assert(
        std::is_enum<E>::value &&
            std::is_same<uint32_t,
                         typename std::underlying_type<E>::type>::value,
        "Only enums backed by uint32_t are accepted.");
    WriteUint32(static_cast<uint32_t>(value));
  }

 private:
  // Transfer is split into two phases: scanning the transferables so that we
  // don't have to serialize the data (just an index), and finalizing (to
  // neuter objects in the source context).
  // This separation is required by the spec (it prevents neutering from
  // happening if there's a failure earlier in serialization).
  void PrepareTransfer(ExceptionState&);
  void FinalizeTransfer(ExceptionState&);

  // Shared between File and FileList logic; does not write a leading tag.
  bool WriteFile(File*, ExceptionState&);

  // v8::ValueSerializer::Delegate
  void ThrowDataCloneError(v8::Local<v8::String> message) override;
  v8::Maybe<bool> WriteHostObject(v8::Isolate*,
                                  v8::Local<v8::Object> message) override;
  v8::Maybe<uint32_t> GetSharedArrayBufferId(
      v8::Isolate*,
      v8::Local<v8::SharedArrayBuffer>) override;

  v8::Maybe<uint32_t> GetWasmModuleTransferId(
      v8::Isolate*,
      v8::Local<v8::WasmCompiledModule>) override;
  // Reallocates memory at |ptr| to the new size and returns the new pointer or
  // nullptr on failure. |actual_size| will hold the actual size of allocation
  // requested.
  void* ReallocateBufferMemory(void* old_buffer,
                               size_t,
                               size_t* actual_size) override;
  void FreeBufferMemory(void* buffer) override;

  Member<ScriptState> script_state_;
  scoped_refptr<SerializedScriptValue> serialized_script_value_;
  v8::ValueSerializer serializer_;
  const Transferables* transferables_ = nullptr;
  const ExceptionState* exception_state_ = nullptr;
  WebBlobInfoArray* blob_info_array_ = nullptr;
  SharedArrayBufferArray shared_array_buffers_;
  Options::WasmSerializationPolicy wasm_policy_;
  bool for_storage_ = false;
#if DCHECK_IS_ON()
  bool serialize_invoked_ = false;
#endif
};

// For code testing V8ScriptValueSerializer
scoped_refptr<SerializedScriptValue> SerializedValue(
    const Vector<uint8_t>& bytes);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_BINDINGS_CORE_V8_SERIALIZATION_V8_SCRIPT_VALUE_SERIALIZER_H_
