// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_PAYMENTS_PAYMENT_EVENT_DATA_CONVERSION_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_PAYMENTS_PAYMENT_EVENT_DATA_CONVERSION_H_

#include "third_party/blink/renderer/modules/payments/can_make_payment_event_init.h"
#include "third_party/blink/renderer/modules/payments/payment_request_event_init.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace blink {

class CanMakePaymentEventInit;
class PaymentRequestEventInit;
class ScriptState;
struct WebCanMakePaymentEventData;
struct WebPaymentRequestEventData;

class MODULES_EXPORT PaymentEventDataConversion {
  STATIC_ONLY(PaymentEventDataConversion);

 public:
  static CanMakePaymentEventInit* ToCanMakePaymentEventInit(
      ScriptState*,
      const WebCanMakePaymentEventData&);
  static PaymentRequestEventInit* ToPaymentRequestEventInit(
      ScriptState*,
      const WebPaymentRequestEventData&);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_PAYMENTS_PAYMENT_EVENT_DATA_CONVERSION_H_
