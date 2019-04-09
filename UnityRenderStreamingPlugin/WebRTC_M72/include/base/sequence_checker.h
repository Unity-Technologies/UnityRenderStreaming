// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_SEQUENCE_CHECKER_H_
#define BASE_SEQUENCE_CHECKER_H_

#include "base/compiler_specific.h"
#include "base/logging.h"
#include "base/sequence_checker_impl.h"

// SequenceChecker is a helper class used to help verify that some methods of a
// class are called sequentially (for thread-safety).
//
// Use the macros below instead of the SequenceChecker directly so that the
// unused member doesn't result in an extra byte (four when padded) per
// instance in production.
//
// This class is much prefered to ThreadChecker for thread-safety checks.
// ThreadChecker should only be used for classes that are truly thread-affine
// (use thread-local-storage or a third-party API that does).
//
// Usage:
//   class MyClass {
//    public:
//     MyClass() {
//       // It's sometimes useful to detach on construction for objects that are
//       // constructed in one place and forever after used from another
//       // sequence.
//       DETACH_FROM_SEQUENCE(my_sequence_checker_);
//     }
//
//     ~MyClass() {
//       // SequenceChecker doesn't automatically check it's destroyed on origin
//       // sequence for the same reason it's sometimes detached in the
//       // constructor. It's okay to destroy off sequence if the owner
//       // otherwise knows usage on the associated sequence is done. If you're
//       // not detaching in the constructor, you probably want to explicitly
//       // check in the destructor.
//       DCHECK_CALLED_ON_VALID_SEQUENCE(my_sequence_checker_);
//     }
//     void MyMethod() {
//       DCHECK_CALLED_ON_VALID_SEQUENCE(my_sequence_checker_);
//       ... (do stuff) ...
//     }
//
//    private:
//     SEQUENCE_CHECKER(my_sequence_checker_);
//   }

#if DCHECK_IS_ON()
#define SEQUENCE_CHECKER(name) base::SequenceChecker name
#define DCHECK_CALLED_ON_VALID_SEQUENCE(name) \
  DCHECK((name).CalledOnValidSequence())
#define DETACH_FROM_SEQUENCE(name) (name).DetachFromSequence()
#else  // DCHECK_IS_ON()
#define SEQUENCE_CHECKER(name)
#define DCHECK_CALLED_ON_VALID_SEQUENCE(name) EAT_STREAM_PARAMETERS
#define DETACH_FROM_SEQUENCE(name)
#endif  // DCHECK_IS_ON()

namespace base {

// Do nothing implementation, for use in release mode.
//
// Note: You should almost always use the SequenceChecker class (through the
// above macros) to get the right version for your build configuration.
class SequenceCheckerDoNothing {
 public:
  SequenceCheckerDoNothing() = default;
  bool CalledOnValidSequence() const WARN_UNUSED_RESULT { return true; }
  void DetachFromSequence() {}

 private:
  DISALLOW_COPY_AND_ASSIGN(SequenceCheckerDoNothing);
};

#if DCHECK_IS_ON()
class SequenceChecker : public SequenceCheckerImpl {
};
#else
class SequenceChecker : public SequenceCheckerDoNothing {
};
#endif  // DCHECK_IS_ON()

}  // namespace base

#endif  // BASE_SEQUENCE_CHECKER_H_
