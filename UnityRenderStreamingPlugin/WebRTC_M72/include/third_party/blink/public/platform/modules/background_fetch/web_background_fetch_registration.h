// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_MODULES_BACKGROUND_FETCH_WEB_BACKGROUND_FETCH_REGISTRATION_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_MODULES_BACKGROUND_FETCH_WEB_BACKGROUND_FETCH_REGISTRATION_H_

#include "third_party/blink/public/platform/modules/background_fetch/background_fetch.mojom-shared.h"
#include "third_party/blink/public/platform/web_common.h"
#include "third_party/blink/public/platform/web_string.h"

namespace blink {

// Represents a BackgroundFetchRegistration object, added mainly for layering.
// Analogous to the following structure in the spec:
// https://wicg.github.io/background-fetch/#background-fetch-registration
struct WebBackgroundFetchRegistration {
  WebBackgroundFetchRegistration(
      const WebString& developer_id,
      const WebString& unique_id,
      uint64_t upload_total,
      uint64_t uploaded,
      uint64_t download_total,
      uint64_t downloaded,
      mojom::BackgroundFetchResult result,
      mojom::BackgroundFetchFailureReason failure_reason)
      : developer_id(developer_id),
        unique_id(unique_id),
        upload_total(upload_total),
        uploaded(uploaded),
        download_total(download_total),
        downloaded(downloaded),
        result(result),
        failure_reason(failure_reason) {}

  ~WebBackgroundFetchRegistration() = default;

  WebString developer_id;
  WebString unique_id;
  uint64_t upload_total;
  uint64_t uploaded;
  uint64_t download_total;
  uint64_t downloaded;
  mojom::BackgroundFetchResult result;
  mojom::BackgroundFetchFailureReason failure_reason;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_MODULES_BACKGROUND_FETCH_WEB_BACKGROUND_FETCH_REGISTRATION_H_
