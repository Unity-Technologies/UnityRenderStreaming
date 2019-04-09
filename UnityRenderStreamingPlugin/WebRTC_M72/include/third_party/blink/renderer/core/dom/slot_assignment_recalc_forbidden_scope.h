// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_DOM_SLOT_ASSIGNMENT_RECALC_FORBIDDEN_SCOPE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_DOM_SLOT_ASSIGNMENT_RECALC_FORBIDDEN_SCOPE_H_

#include "base/macros.h"
#include "third_party/blink/renderer/core/dom/document.h"

namespace blink {

#if DCHECK_IS_ON()
class SlotAssignmentRecalcForbiddenScope {
  STACK_ALLOCATED();

 public:
  explicit SlotAssignmentRecalcForbiddenScope(Document& document)
      : count_(document.SlotAssignmentRecalcForbiddenRecursionDepth()) {
    ++count_;
  }

  ~SlotAssignmentRecalcForbiddenScope() { --count_; }

 private:
  unsigned& count_;
  DISALLOW_COPY_AND_ASSIGN(SlotAssignmentRecalcForbiddenScope);
};
#else
class SlotAssignmentRecalcForbiddenScope {
  STACK_ALLOCATED();

 public:
  explicit SlotAssignmentRecalcForbiddenScope(Document&) {}

 private:
  DISALLOW_COPY_AND_ASSIGN(SlotAssignmentRecalcForbiddenScope);
};
#endif

}  // namespace blink

#endif
