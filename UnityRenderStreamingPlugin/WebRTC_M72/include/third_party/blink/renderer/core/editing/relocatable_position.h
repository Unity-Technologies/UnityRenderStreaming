// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_RELOCATABLE_POSITION_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_RELOCATABLE_POSITION_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/range.h"

namespace blink {

// |RelocatablePosition| is a helper class for keeping track of a |Position| in
// a document upon DOM tree changes even if the given |Position|'s original
// anchor node is moved out of document. The class is implemented by using a
// temporary |Range| object to keep track of the |Position|, and disposing the
// |Range| when out of scope.
class CORE_EXPORT RelocatablePosition final {
  STACK_ALLOCATED();

 public:
  explicit RelocatablePosition(const Position&);
  ~RelocatablePosition();

  Position GetPosition() const;

 private:
  const Member<Range> range_;

  DISALLOW_COPY_AND_ASSIGN(RelocatablePosition);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_RELOCATABLE_POSITION_H_
