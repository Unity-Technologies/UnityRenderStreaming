// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TOOLS_IPC_FUZZER_MESSAGE_LIB_MESSAGE_FILE_H_
#define TOOLS_IPC_FUZZER_MESSAGE_LIB_MESSAGE_FILE_H_

#include <memory>
#include <vector>

#include "base/files/file_path.h"
#include "base/macros.h"
#include "ipc/ipc_message.h"

namespace ipc_fuzzer {

using MessageVector = std::vector<std::unique_ptr<IPC::Message>>;

class MessageFile {
 public:
  static bool Read(const base::FilePath& path, MessageVector* messages);
  static bool Write(const base::FilePath& path, const MessageVector& messages);

 private:
  DISALLOW_COPY_AND_ASSIGN(MessageFile);
};

}  // namespace ipc_fuzzer

#endif  // TOOLS_IPC_FUZZER_MESSAGE_LIB_MESSAGE_FILE_H_
