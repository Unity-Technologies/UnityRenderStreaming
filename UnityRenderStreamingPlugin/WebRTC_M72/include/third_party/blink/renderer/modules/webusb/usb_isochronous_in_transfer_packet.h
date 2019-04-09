// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBUSB_USB_ISOCHRONOUS_IN_TRANSFER_PACKET_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBUSB_USB_ISOCHRONOUS_IN_TRANSFER_PACKET_H_

#include "third_party/blink/renderer/core/typed_arrays/dom_data_view.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class USBIsochronousInTransferPacket final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static USBIsochronousInTransferPacket* Create(const String& status) {
    return new USBIsochronousInTransferPacket(status, nullptr);
  }

  static USBIsochronousInTransferPacket* Create(const String& status,
                                                DOMDataView* data) {
    return new USBIsochronousInTransferPacket(status, data);
  }

  ~USBIsochronousInTransferPacket() override = default;

  String status() const { return status_; }
  DOMDataView* data() const { return data_; }

  void Trace(blink::Visitor* visitor) override {
    visitor->Trace(data_);
    ScriptWrappable::Trace(visitor);
  }

 private:
  USBIsochronousInTransferPacket(const String& status, DOMDataView* data)
      : status_(status), data_(data) {}

  const String status_;
  const Member<DOMDataView> data_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBUSB_USB_ISOCHRONOUS_IN_TRANSFER_PACKET_H_
