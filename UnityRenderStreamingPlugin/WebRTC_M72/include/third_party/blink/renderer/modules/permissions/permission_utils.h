// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_PERMISSIONS_PERMISSION_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_PERMISSIONS_PERMISSION_UTILS_H_

#include "third_party/blink/public/platform/modules/permissions/permission.mojom-blink.h"

namespace blink {

class ExecutionContext;

void ConnectToPermissionService(ExecutionContext*,
                                mojom::blink::PermissionServiceRequest);

mojom::blink::PermissionDescriptorPtr CreatePermissionDescriptor(
    mojom::blink::PermissionName);

mojom::blink::PermissionDescriptorPtr CreateMidiPermissionDescriptor(
    bool sysex);

mojom::blink::PermissionDescriptorPtr CreateClipboardPermissionDescriptor(
    mojom::blink::PermissionName,
    bool allow_without_gesture);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_PERMISSIONS_PERMISSION_UTILS_H_
