// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_PRESENTATION_MOCK_PRESENTATION_SERVICE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_PRESENTATION_MOCK_PRESENTATION_SERVICE_H_

#include "testing/gmock/include/gmock/gmock.h"
#include "third_party/blink/public/mojom/presentation/presentation.mojom-blink.h"

namespace blink {

class MockPresentationService : public mojom::blink::PresentationService {
 public:
  void SetController(mojom::blink::PresentationControllerPtr) override {}
  void SetReceiver(mojom::blink::PresentationReceiverPtr) override {}
  MOCK_METHOD1(SetDefaultPresentationUrls, void(const Vector<KURL>&));
  MOCK_METHOD1(ListenForScreenAvailability, void(const KURL&));
  MOCK_METHOD1(StopListeningForScreenAvailability, void(const KURL&));

  // TODO(crbug.com/729950): Use MOCK_METHOD directly once GMock gets the
  // move-only type support.
  void StartPresentation(const Vector<KURL>& presentation_urls,
                         StartPresentationCallback callback) override {
    StartPresentationInternal(presentation_urls, callback);
  }
  MOCK_METHOD2(StartPresentationInternal,
               void(const Vector<KURL>&, StartPresentationCallback&));

  void ReconnectPresentation(const Vector<KURL>& presentation_urls,
                             const String& presentation_id,
                             ReconnectPresentationCallback callback) override {
    ReconnectPresentationInternal(presentation_urls, presentation_id, callback);
  }
  MOCK_METHOD3(ReconnectPresentationInternal,
               void(const Vector<KURL>& presentation_urls,
                    const String& presentation_id,
                    ReconnectPresentationCallback&));

  MOCK_METHOD2(CloseConnection, void(const KURL&, const String&));
  MOCK_METHOD2(Terminate, void(const KURL&, const String&));
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_PRESENTATION_MOCK_PRESENTATION_SERVICE_H_
