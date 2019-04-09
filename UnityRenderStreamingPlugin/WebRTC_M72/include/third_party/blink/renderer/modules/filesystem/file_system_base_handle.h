// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_FILESYSTEM_FILE_SYSTEM_BASE_HANDLE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_FILESYSTEM_FILE_SYSTEM_BASE_HANDLE_H_

#include "third_party/blink/renderer/modules/filesystem/entry_base.h"

namespace blink {
class FileSystemDirectoryHandle;
class ScriptPromise;
class ScriptState;

class FileSystemBaseHandle : public EntryBase {
  DEFINE_WRAPPERTYPEINFO();

 public:
  explicit FileSystemBaseHandle(DOMFileSystemBase*, const String& full_path);

  ScriptPromise getParent(ScriptState*);
  ScriptPromise moveTo(ScriptState*,
                       FileSystemDirectoryHandle* parent,
                       const String& name = String());
  ScriptPromise copyTo(ScriptState*,
                       FileSystemDirectoryHandle* parent,
                       const String& name = String());
  ScriptPromise remove(ScriptState*);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_FILESYSTEM_FILE_SYSTEM_BASE_HANDLE_H_
