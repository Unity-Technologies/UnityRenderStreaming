// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_COMMON_SERVICE_WORKER_SERVICE_WORKER_UTILS_H_
#define THIRD_PARTY_BLINK_PUBLIC_COMMON_SERVICE_WORKER_SERVICE_WORKER_UTILS_H_

#include "third_party/blink/public/common/common_export.h"

namespace blink {

class ServiceWorkerUtils {
 public:
  // Whether the new service worker glue for NetworkService is enabled (i.e.,
  // the NetworkService or ServiceWorkerServicification feature is enabled).
  // TODO(crbug.com/715640): Remove this after non-NetS13nSW is removed.
  static bool BLINK_COMMON_EXPORT IsServicificationEnabled();

  static bool BLINK_COMMON_EXPORT IsImportedScriptUpdateCheckEnabled();
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_COMMON_SERVICE_WORKER_SERVICE_WORKER_UTILS_H_
