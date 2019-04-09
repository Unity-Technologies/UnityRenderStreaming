// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FETCH_FORM_DATA_BYTES_CONSUMER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FETCH_FORM_DATA_BYTES_CONSUMER_H_

#include "base/memory/scoped_refptr.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/fetch/bytes_consumer.h"
#include "third_party/blink/renderer/platform/bindings/trace_wrapper_member.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

namespace blink {

class DOMArrayBuffer;
class DOMArrayBufferView;
class EncodedFormData;

class FormDataBytesConsumer final : public BytesConsumer {
 public:
  explicit CORE_EXPORT FormDataBytesConsumer(const String&);
  explicit CORE_EXPORT FormDataBytesConsumer(DOMArrayBuffer*);
  explicit CORE_EXPORT FormDataBytesConsumer(DOMArrayBufferView*);
  CORE_EXPORT FormDataBytesConsumer(const void* data, wtf_size_t);
  CORE_EXPORT FormDataBytesConsumer(ExecutionContext*,
                                    scoped_refptr<EncodedFormData>);
  CORE_EXPORT FormDataBytesConsumer(ExecutionContext*,
                                    scoped_refptr<EncodedFormData>,
                                    BytesConsumer* consumer_for_testing);

  CORE_EXPORT static FormDataBytesConsumer* CreateForTesting(
      ExecutionContext* execution_context,
      scoped_refptr<EncodedFormData> form_data,
      BytesConsumer* consumer) {
    return MakeGarbageCollected<FormDataBytesConsumer>(
        execution_context, std::move(form_data), consumer);
  }

  // BytesConsumer implementation
  Result BeginRead(const char** buffer, size_t* available) override {
    return impl_->BeginRead(buffer, available);
  }
  Result EndRead(size_t read_size) override {
    return impl_->EndRead(read_size);
  }
  scoped_refptr<BlobDataHandle> DrainAsBlobDataHandle(
      BlobSizePolicy policy) override {
    return impl_->DrainAsBlobDataHandle(policy);
  }
  scoped_refptr<EncodedFormData> DrainAsFormData() override {
    return impl_->DrainAsFormData();
  }
  void SetClient(BytesConsumer::Client* client) override {
    impl_->SetClient(client);
  }
  void ClearClient() override { impl_->ClearClient(); }
  void Cancel() override { impl_->Cancel(); }
  PublicState GetPublicState() const override {
    return impl_->GetPublicState();
  }
  Error GetError() const override { return impl_->GetError(); }
  String DebugName() const override { return impl_->DebugName(); }

  void Trace(blink::Visitor* visitor) override {
    visitor->Trace(impl_);
    BytesConsumer::Trace(visitor);
  }

 private:
  static BytesConsumer* GetImpl(ExecutionContext*,
                                scoped_refptr<EncodedFormData>,
                                BytesConsumer* consumer_for_testing);

  const TraceWrapperMember<BytesConsumer> impl_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FETCH_FORM_DATA_BYTES_CONSUMER_H_
