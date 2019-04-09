// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBUSB_USB_ISOCHRONOUS_OUT_TRANSFER_PACKET_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBUSB_USB_ISOCHRONOUS_OUT_TRANSFER_PACKET_H_

#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class USBIsochronousOutTransferPacket final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static USBIsochronousOutTransferPacket* Create(const String& status) {
    return new USBIsochronousOutTransferPacket(status, 0);
  }

  static USBIsochronousOutTransferPacket* Create(const String& status,
                                                 unsigned bytes_written) {
    return new USBIsochronousOutTransferPacket(status, bytes_written);
  }

  USBIsochronousOutTransferPacket(const String& status, unsigned bytes_written)
      : status_(status), bytes_written_(bytes_written) {}

  ~USBIsochronousOutTransferPacket() override = default;

  String status() const { return status_; }
  unsigned bytesWritten() const { return bytes_written_; }

 private:
  const String status_;
  const unsigned bytes_written_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBUSB_USB_ISOCHRONOUS_OUT_TRANSFER_PACKET_H_
