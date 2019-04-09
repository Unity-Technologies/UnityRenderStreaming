// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_BLOB_TESTING_FAKE_BLOB_REGISTRY_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_BLOB_TESTING_FAKE_BLOB_REGISTRY_H_

#include "third_party/blink/public/mojom/blob/blob_registry.mojom-blink.h"

namespace blink {

// Mocked BlobRegistry implementation for testing. Simply keeps track of all
// blob registrations and blob lookup requests, binding each blob request to a
// FakeBlob instance with the correct uuid.
class FakeBlobRegistry : public mojom::blink::BlobRegistry {
 public:
  void Register(mojom::blink::BlobRequest,
                const String& uuid,
                const String& content_type,
                const String& content_disposition,
                Vector<mojom::blink::DataElementPtr> elements,
                RegisterCallback) override;

  void RegisterFromStream(const String& content_type,
                          const String& content_disposition,
                          uint64_t expected_length,
                          mojo::ScopedDataPipeConsumerHandle,
                          mojom::blink::ProgressClientAssociatedPtrInfo,
                          RegisterFromStreamCallback) override;

  void GetBlobFromUUID(mojom::blink::BlobRequest,
                       const String& uuid,
                       GetBlobFromUUIDCallback) override;

  void URLStoreForOrigin(const scoped_refptr<const SecurityOrigin>&,
                         mojom::blink::BlobURLStoreAssociatedRequest) override;

  struct Registration {
    String uuid;
    String content_type;
    String content_disposition;
    Vector<mojom::blink::DataElementPtr> elements;
  };
  Vector<Registration> registrations;

  struct BindingRequest {
    String uuid;
  };
  Vector<BindingRequest> binding_requests;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_BLOB_TESTING_FAKE_BLOB_REGISTRY_H_
