// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_STATE_MACHINES_STATE_MACHINE_TEST_UTIL_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_STATE_MACHINES_STATE_MACHINE_TEST_UTIL_H_

#include "base/macros.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/platform/wtf/text/unicode.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class BackwardGraphemeBoundaryStateMachine;
class ForwardGraphemeBoundaryStateMachine;

class GraphemeStateMachineTestBase : public testing::Test {
 protected:
  GraphemeStateMachineTestBase() = default;
  ~GraphemeStateMachineTestBase() override = default;

  Vector<UChar32> AsCodePoints() { return Vector<UChar32>(); }

  template <typename... Args>
  Vector<UChar32> AsCodePoints(Args... args) {
    UChar32 code_points[] = {args...};
    Vector<UChar32> result(sizeof...(args));
    for (wtf_size_t index = 0; index < sizeof...(args); ++index)
      result[index] = code_points[index];
    return result;
  }

  // Processes the |machine| with preceding/following code points.
  // The result string represents the output sequence of the state machine.
  // Each character represents returned state of each action.
  // I : Invalid
  // R : NeedMoreCodeUnit (Repeat)
  // S : NeedFollowingCodeUnit (Switch)
  // F : Finished
  //
  // For example, if a state machine returns following sequence:
  //   NeedMoreCodeUnit, NeedFollowingCodeUnit, Finished
  // the returned string will be "RSF".
  String ProcessSequenceBackward(BackwardGraphemeBoundaryStateMachine*,
                                 const Vector<UChar32>& preceding);

  String ProcessSequenceForward(ForwardGraphemeBoundaryStateMachine*,
                                const Vector<UChar32>& preceding,
                                const Vector<UChar32>& following);

 private:
  DISALLOW_COPY_AND_ASSIGN(GraphemeStateMachineTestBase);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_STATE_MACHINES_STATE_MACHINE_TEST_UTIL_H_
