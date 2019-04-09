/*
 * Copyright (C) 2006, 2007, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2009 Igalia S.L.
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

// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_COMMANDS_MOVE_COMMANDS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_COMMANDS_MOVE_COMMANDS_H_

#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

namespace blink {

class Event;
class LocalFrame;

enum class EditorCommandSource;
enum class SelectionModifyAlteration;
enum class SelectionModifyVerticalDirection;

// This class provides static functions about commands related to move.
class MoveCommands {
  STATIC_ONLY(MoveCommands);

 public:
  // Returns |bool| value for Document#execCommand().
  static bool ExecuteMoveBackward(LocalFrame&,
                                  Event*,
                                  EditorCommandSource,
                                  const String&);
  static bool ExecuteMoveBackwardAndModifySelection(LocalFrame&,
                                                    Event*,
                                                    EditorCommandSource,
                                                    const String&);
  static bool ExecuteMoveDown(LocalFrame&,
                              Event*,
                              EditorCommandSource,
                              const String&);
  static bool ExecuteMoveDownAndModifySelection(LocalFrame&,
                                                Event*,
                                                EditorCommandSource,
                                                const String&);
  static bool ExecuteMoveForward(LocalFrame&,
                                 Event*,
                                 EditorCommandSource,
                                 const String&);
  static bool ExecuteMoveForwardAndModifySelection(LocalFrame&,
                                                   Event*,
                                                   EditorCommandSource,
                                                   const String&);
  static bool ExecuteMoveLeft(LocalFrame&,
                              Event*,
                              EditorCommandSource,
                              const String&);
  static bool ExecuteMoveLeftAndModifySelection(LocalFrame&,
                                                Event*,
                                                EditorCommandSource,
                                                const String&);
  static bool ExecuteMovePageDown(LocalFrame&,
                                  Event*,
                                  EditorCommandSource,
                                  const String&);
  static bool ExecuteMovePageDownAndModifySelection(LocalFrame&,
                                                    Event*,
                                                    EditorCommandSource,
                                                    const String&);
  static bool ExecuteMovePageUp(LocalFrame&,
                                Event*,
                                EditorCommandSource,
                                const String&);
  static bool ExecuteMovePageUpAndModifySelection(LocalFrame&,
                                                  Event*,
                                                  EditorCommandSource,
                                                  const String&);
  static bool ExecuteMoveParagraphBackward(LocalFrame&,
                                           Event*,
                                           EditorCommandSource,
                                           const String&);
  static bool ExecuteMoveParagraphBackwardAndModifySelection(
      LocalFrame&,
      Event*,
      EditorCommandSource,
      const String&);
  static bool ExecuteMoveParagraphForward(LocalFrame&,
                                          Event*,
                                          EditorCommandSource,
                                          const String&);
  static bool ExecuteMoveParagraphForwardAndModifySelection(LocalFrame&,
                                                            Event*,
                                                            EditorCommandSource,
                                                            const String&);
  static bool ExecuteMoveRight(LocalFrame&,
                               Event*,
                               EditorCommandSource,
                               const String&);
  static bool ExecuteMoveRightAndModifySelection(LocalFrame&,
                                                 Event*,
                                                 EditorCommandSource,
                                                 const String&);
  static bool ExecuteMoveToBeginningOfDocument(LocalFrame&,
                                               Event*,
                                               EditorCommandSource,
                                               const String&);
  static bool ExecuteMoveToBeginningOfDocumentAndModifySelection(
      LocalFrame&,
      Event*,
      EditorCommandSource,
      const String&);
  static bool ExecuteMoveToBeginningOfLine(LocalFrame&,
                                           Event*,
                                           EditorCommandSource,
                                           const String&);
  static bool ExecuteMoveToBeginningOfLineAndModifySelection(
      LocalFrame&,
      Event*,
      EditorCommandSource,
      const String&);
  static bool ExecuteMoveToBeginningOfParagraph(LocalFrame&,
                                                Event*,
                                                EditorCommandSource,
                                                const String&);
  static bool ExecuteMoveToBeginningOfParagraphAndModifySelection(
      LocalFrame&,
      Event*,
      EditorCommandSource,
      const String&);
  static bool ExecuteMoveToBeginningOfSentence(LocalFrame&,
                                               Event*,
                                               EditorCommandSource,
                                               const String&);
  static bool ExecuteMoveToBeginningOfSentenceAndModifySelection(
      LocalFrame&,
      Event*,
      EditorCommandSource,
      const String&);
  static bool ExecuteMoveToEndOfDocument(LocalFrame&,
                                         Event*,
                                         EditorCommandSource,
                                         const String&);
  static bool ExecuteMoveToEndOfDocumentAndModifySelection(LocalFrame&,
                                                           Event*,
                                                           EditorCommandSource,
                                                           const String&);
  static bool ExecuteMoveToEndOfLine(LocalFrame&,
                                     Event*,
                                     EditorCommandSource,
                                     const String&);
  static bool ExecuteMoveToEndOfLineAndModifySelection(LocalFrame&,
                                                       Event*,
                                                       EditorCommandSource,
                                                       const String&);
  static bool ExecuteMoveToEndOfParagraph(LocalFrame&,
                                          Event*,
                                          EditorCommandSource,
                                          const String&);
  static bool ExecuteMoveToEndOfParagraphAndModifySelection(LocalFrame&,
                                                            Event*,
                                                            EditorCommandSource,
                                                            const String&);
  static bool ExecuteMoveToEndOfSentence(LocalFrame&,
                                         Event*,
                                         EditorCommandSource,
                                         const String&);
  static bool ExecuteMoveToEndOfSentenceAndModifySelection(LocalFrame&,
                                                           Event*,
                                                           EditorCommandSource,
                                                           const String&);
  static bool ExecuteMoveToLeftEndOfLine(LocalFrame&,
                                         Event*,
                                         EditorCommandSource,
                                         const String&);
  static bool ExecuteMoveToLeftEndOfLineAndModifySelection(LocalFrame&,
                                                           Event*,
                                                           EditorCommandSource,
                                                           const String&);
  static bool ExecuteMoveToRightEndOfLine(LocalFrame&,
                                          Event*,
                                          EditorCommandSource,
                                          const String&);
  static bool ExecuteMoveToRightEndOfLineAndModifySelection(LocalFrame&,
                                                            Event*,
                                                            EditorCommandSource,
                                                            const String&);
  static bool ExecuteMoveUp(LocalFrame&,
                            Event*,
                            EditorCommandSource,
                            const String&);
  static bool ExecuteMoveUpAndModifySelection(LocalFrame&,
                                              Event*,
                                              EditorCommandSource,
                                              const String&);
  static bool ExecuteMoveWordBackward(LocalFrame&,
                                      Event*,
                                      EditorCommandSource,
                                      const String&);
  static bool ExecuteMoveWordBackwardAndModifySelection(LocalFrame&,
                                                        Event*,
                                                        EditorCommandSource,
                                                        const String&);
  static bool ExecuteMoveWordForward(LocalFrame&,
                                     Event*,
                                     EditorCommandSource,
                                     const String&);
  static bool ExecuteMoveWordForwardAndModifySelection(LocalFrame&,
                                                       Event*,
                                                       EditorCommandSource,
                                                       const String&);
  static bool ExecuteMoveWordLeft(LocalFrame&,
                                  Event*,
                                  EditorCommandSource,
                                  const String&);
  static bool ExecuteMoveWordLeftAndModifySelection(LocalFrame&,
                                                    Event*,
                                                    EditorCommandSource,
                                                    const String&);
  static bool ExecuteMoveWordRight(LocalFrame&,
                                   Event*,
                                   EditorCommandSource,
                                   const String&);
  static bool ExecuteMoveWordRightAndModifySelection(LocalFrame&,
                                                     Event*,
                                                     EditorCommandSource,
                                                     const String&);

 private:
  static unsigned VerticalScrollDistance(LocalFrame&);

  // Returns true if selection is modified.
  static bool ModifySelectionWithPageGranularity(
      LocalFrame&,
      SelectionModifyAlteration,
      unsigned,
      SelectionModifyVerticalDirection);
};

}  // namespace blink

#endif
