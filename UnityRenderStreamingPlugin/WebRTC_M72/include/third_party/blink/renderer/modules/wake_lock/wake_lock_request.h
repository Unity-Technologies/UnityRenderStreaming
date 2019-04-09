// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WAKE_LOCK_WAKE_LOCK_REQUEST_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WAKE_LOCK_WAKE_LOCK_REQUEST_H_

#include "third_party/blink/renderer/modules/wake_lock/wake_lock.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"

namespace blink {

class WakeLockRequest final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  explicit WakeLockRequest(WakeLock*);
  ~WakeLockRequest() override;

  void Trace(blink::Visitor*) override;

  void cancel();

 private:
  Member<WakeLock> owner_wake_lock_;
  bool cancelled_ = false;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WAKE_LOCK_WAKE_LOCK_REQUEST_H_
