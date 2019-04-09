// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_BLUETOOTH_BLUETOOTH_REMOTE_GATT_DESCRIPTOR_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_BLUETOOTH_BLUETOOTH_REMOTE_GATT_DESCRIPTOR_H_

#include <memory>
#include "third_party/blink/renderer/core/typed_arrays/dom_array_piece.h"
#include "third_party/blink/renderer/core/typed_arrays/dom_data_view.h"
#include "third_party/blink/renderer/modules/bluetooth/bluetooth.h"
#include "third_party/blink/renderer/modules/bluetooth/bluetooth_remote_gatt_characteristic.h"
#include "third_party/blink/renderer/modules/bluetooth/bluetooth_remote_gatt_service.h"
#include "third_party/blink/renderer/modules/event_target_modules.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class BluetoothRemoteGATTCharacteristic;
class ScriptPromise;
class ScriptState;

// BluetoothRemoteGATTDescriptor represents a GATT Descriptor, which is
// a basic data element that provides further information about a peripheral's
// characteristic.
class BluetoothRemoteGATTDescriptor final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  BluetoothRemoteGATTDescriptor(
      mojom::blink::WebBluetoothRemoteGATTDescriptorPtr,
      BluetoothRemoteGATTCharacteristic*);

  static BluetoothRemoteGATTDescriptor* Create(
      mojom::blink::WebBluetoothRemoteGATTDescriptorPtr,
      BluetoothRemoteGATTCharacteristic*);

  // IDL exposed interface:
  BluetoothRemoteGATTCharacteristic* characteristic() {
    return characteristic_;
  }
  String uuid() { return descriptor_->uuid; }
  DOMDataView* value() const { return value_; }
  ScriptPromise readValue(ScriptState*);
  ScriptPromise writeValue(ScriptState*, const DOMArrayPiece&);

  // Interface required by garbage collection.
  void Trace(blink::Visitor*) override;

 private:
  friend class DescriptorReadValueCallback;

  BluetoothRemoteGATTServer* GetGatt() { return characteristic_->GetGatt(); }
  mojom::blink::WebBluetoothService* GetService() {
    return characteristic_->device_->GetBluetooth()->Service();
  }

  void ReadValueCallback(ScriptPromiseResolver*,
                         mojom::blink::WebBluetoothResult,
                         const base::Optional<Vector<uint8_t>>&);

  void WriteValueCallback(ScriptPromiseResolver*,
                          const Vector<uint8_t>&,
                          mojom::blink::WebBluetoothResult);

  DOMException* CreateInvalidDescriptorError();

  mojom::blink::WebBluetoothRemoteGATTDescriptorPtr descriptor_;
  Member<BluetoothRemoteGATTCharacteristic> characteristic_;
  Member<DOMDataView> value_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_BLUETOOTH_BLUETOOTH_REMOTE_GATT_DESCRIPTOR_H_
