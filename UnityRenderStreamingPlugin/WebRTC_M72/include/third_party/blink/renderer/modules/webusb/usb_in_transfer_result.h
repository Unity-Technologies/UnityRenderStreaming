// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBUSB_USB_IN_TRANSFER_RESULT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBUSB_USB_IN_TRANSFER_RESULT_H_

#include "third_party/blink/renderer/core/typed_arrays/dom_array_buffer.h"
#include "third_party/blink/renderer/core/typed_arrays/dom_data_view.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class USBInTransferResult final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static USBInTransferResult* Create(const String& status,
                                     const Vector<uint8_t>& data) {
    DOMDataView* data_view = DOMDataView::Create(
        DOMArrayBuffer::Create(data.data(), data.size()), 0, data.size());
    return new USBInTransferResult(status, data_view);
  }

  static USBInTransferResult* Create(const String& status) {
    return new USBInTransferResult(status, nullptr);
  }

  static USBInTransferResult* Create(const String& status, DOMDataView* data) {
    return new USBInTransferResult(status, data);
  }

  USBInTransferResult(const String& status, DOMDataView* data)
      : status_(status), data_(data) {}

  ~USBInTransferResult() override = default;

  String status() const { return status_; }
  DOMDataView* data() const { return data_; }

  void Trace(blink::Visitor* visitor) override {
    visitor->Trace(data_);
    ScriptWrappable::Trace(visitor);
  }

 private:
  const String status_;
  const Member<DOMDataView> data_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBUSB_USB_IN_TRANSFER_RESULT_H_
