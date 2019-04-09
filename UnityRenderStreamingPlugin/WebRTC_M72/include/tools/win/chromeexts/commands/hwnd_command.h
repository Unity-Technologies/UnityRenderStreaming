// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TOOLS_WIN_CHROME_EXTS_COMMANDS_HWND_COMMAND_H_
#define TOOLS_WIN_CHROME_EXTS_COMMANDS_HWND_COMMAND_H_

#include "tools/win/chromeexts/chrome_exts_command.h"

namespace tools {
namespace win {
namespace chromeexts {

class HwndCommand : public ChromeExtsCommand {
 public:
  HwndCommand();
  ~HwndCommand() override;

 protected:
  HRESULT Execute() override;

 private:
  DISALLOW_COPY_AND_ASSIGN(HwndCommand);
};

}  // namespace chromeexts
}  // namespace win
}  // namespace tools

#endif  // TOOLS_WIN_CHROME_EXTS_COMMANDS_HWND_COMMAND_H_
