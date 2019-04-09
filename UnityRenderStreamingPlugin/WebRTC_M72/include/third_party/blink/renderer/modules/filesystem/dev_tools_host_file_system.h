// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_FILESYSTEM_DEV_TOOLS_HOST_FILE_SYSTEM_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_FILESYSTEM_DEV_TOOLS_HOST_FILE_SYSTEM_H_

#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class DOMFileSystem;
class DevToolsHost;

class DevToolsHostFileSystem {
  STATIC_ONLY(DevToolsHostFileSystem);

 public:
  static DOMFileSystem* isolatedFileSystem(DevToolsHost&,
                                           const String& file_system_name,
                                           const String& root_url);
  static void upgradeDraggedFileSystemPermissions(DevToolsHost&,
                                                  DOMFileSystem*);
};

}  // namespace blink

#endif  // !defined(DevToolsHostFileSystem_h)
