// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_INVALIDATION_NODE_INVALIDATION_SETS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_INVALIDATION_NODE_INVALIDATION_SETS_H_

#include "base/macros.h"
#include "third_party/blink/renderer/core/css/invalidation/invalidation_set.h"
#include "third_party/blink/renderer/platform/heap/heap_allocator.h"

namespace blink {

class CORE_EXPORT NodeInvalidationSets final {
 public:
  NodeInvalidationSets() = default;
  NodeInvalidationSets(NodeInvalidationSets&&) = default;
  NodeInvalidationSets& operator=(NodeInvalidationSets&&) = default;

  InvalidationSetVector& Descendants() { return descendants_; }
  const InvalidationSetVector& Descendants() const { return descendants_; }
  InvalidationSetVector& Siblings() { return siblings_; }
  const InvalidationSetVector& Siblings() const { return siblings_; }

 private:
  InvalidationSetVector descendants_;
  InvalidationSetVector siblings_;
  DISALLOW_COPY_AND_ASSIGN(NodeInvalidationSets);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_INVALIDATION_NODE_INVALIDATION_SETS_H_
