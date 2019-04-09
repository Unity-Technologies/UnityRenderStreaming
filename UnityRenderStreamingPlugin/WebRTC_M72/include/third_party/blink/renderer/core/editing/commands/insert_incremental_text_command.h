// Copyright (c) 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_COMMANDS_INSERT_INCREMENTAL_TEXT_COMMAND_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_COMMANDS_INSERT_INCREMENTAL_TEXT_COMMAND_H_

#include "third_party/blink/renderer/core/editing/commands/insert_text_command.h"

namespace blink {

class CORE_EXPORT InsertIncrementalTextCommand final
    : public InsertTextCommand {
 public:
  static InsertIncrementalTextCommand* Create(
      Document&,
      const String&,
      RebalanceType = kRebalanceLeadingAndTrailingWhitespaces);

  InsertIncrementalTextCommand(Document&,
                               const String& text,
                               RebalanceType);

 private:
  void DoApply(EditingState*) override;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_COMMANDS_INSERT_INCREMENTAL_TEXT_COMMAND_H_
