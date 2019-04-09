/*
 * Copyright (C) 2005, 2006, 2008 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_COMMANDS_SPLIT_ELEMENT_COMMAND_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_COMMANDS_SPLIT_ELEMENT_COMMAND_H_

#include "third_party/blink/renderer/core/editing/commands/edit_command.h"

namespace blink {

class SplitElementCommand final : public SimpleEditCommand {
 public:
  static SplitElementCommand* Create(Element* element,
                                     Node* split_point_child) {
    return new SplitElementCommand(element, split_point_child);
  }

  void Trace(blink::Visitor*) override;

 private:
  SplitElementCommand(Element*, Node* split_point_child);

  void DoApply(EditingState*) override;
  void DoUnapply() override;
  void DoReapply() override;
  void ExecuteApply();

  Member<Element> element1_;
  Member<Element> element2_;
  Member<Node> at_child_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_COMMANDS_SPLIT_ELEMENT_COMMAND_H_
