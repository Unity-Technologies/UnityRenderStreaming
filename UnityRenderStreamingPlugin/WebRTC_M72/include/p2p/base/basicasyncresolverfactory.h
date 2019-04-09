/*
 *  Copyright 2018 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef P2P_BASE_BASICASYNCRESOLVERFACTORY_H_
#define P2P_BASE_BASICASYNCRESOLVERFACTORY_H_

#include "api/asyncresolverfactory.h"

namespace webrtc {

class BasicAsyncResolverFactory : public AsyncResolverFactory {
 public:
  rtc::AsyncResolverInterface* Create() override;
};

}  // namespace webrtc

#endif  // P2P_BASE_BASICASYNCRESOLVERFACTORY_H_
