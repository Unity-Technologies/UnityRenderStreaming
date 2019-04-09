// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_MOJO_BLUETOOTH_STRUCT_TRAITS_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_MOJO_BLUETOOTH_STRUCT_TRAITS_H_

#include "device/bluetooth/public/mojom/uuid.mojom-blink.h"
#include "third_party/blink/public/platform/modules/bluetooth/web_bluetooth.mojom-blink.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace mojo {

template <>
struct StructTraits<::blink::mojom::WebBluetoothDeviceIdDataView, WTF::String> {
  static const WTF::String& device_id(const WTF::String& input) {
    return input;
  }

  static bool Read(::blink::mojom::WebBluetoothDeviceIdDataView,
                   WTF::String* output);
};

template <>
struct StructTraits<bluetooth::mojom::UUIDDataView, WTF::String> {
  static const WTF::String& uuid(const WTF::String& input) { return input; }

  static bool Read(bluetooth::mojom::UUIDDataView, WTF::String* output);

  static bool IsNull(const WTF::String& input) { return input.IsNull(); }

  static void SetToNull(WTF::String* output);
};

}  // namespace mojo

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_MOJO_BLUETOOTH_STRUCT_TRAITS_H_
