// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBUSB_USB_INTERFACE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBUSB_USB_INTERFACE_H_

#include "device/usb/public/mojom/device.mojom-blink.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/heap.h"

namespace blink {

class ExceptionState;
class USBAlternateInterface;
class USBConfiguration;
class USBDevice;

class USBInterface : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static USBInterface* Create(const USBConfiguration*,
                              wtf_size_t interface_index);
  static USBInterface* Create(const USBConfiguration*,
                              uint8_t interface_number,
                              ExceptionState&);

  USBInterface(const USBDevice*,
               wtf_size_t configuration_index,
               wtf_size_t interface_index);

  const device::mojom::blink::UsbInterfaceInfo& Info() const;

  uint8_t interfaceNumber() const { return Info().interface_number; }
  USBAlternateInterface* alternate() const;
  HeapVector<Member<USBAlternateInterface>> alternates() const;
  bool claimed() const;

  void Trace(blink::Visitor*) override;

 private:
  Member<const USBDevice> device_;
  const wtf_size_t configuration_index_;
  const wtf_size_t interface_index_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBUSB_USB_INTERFACE_H_
