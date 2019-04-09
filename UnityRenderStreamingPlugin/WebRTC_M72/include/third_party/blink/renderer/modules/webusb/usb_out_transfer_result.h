// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBUSB_USB_OUT_TRANSFER_RESULT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBUSB_USB_OUT_TRANSFER_RESULT_H_

#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class USBOutTransferResult final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static USBOutTransferResult* Create(const String& status) {
    return new USBOutTransferResult(status, 0);
  }

  static USBOutTransferResult* Create(const String& status,
                                      unsigned bytes_written) {
    return new USBOutTransferResult(status, bytes_written);
  }

  USBOutTransferResult(const String& status, unsigned bytes_written)
      : status_(status), bytes_written_(bytes_written) {}

  ~USBOutTransferResult() override = default;

  String status() const { return status_; }
  unsigned bytesWritten() const { return bytes_written_; }

 private:
  const String status_;
  const unsigned bytes_written_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBUSB_USB_OUT_TRANSFER_RESULT_H_
