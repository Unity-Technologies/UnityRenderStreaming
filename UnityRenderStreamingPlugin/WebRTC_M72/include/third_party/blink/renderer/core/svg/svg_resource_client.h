// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_RESOURCE_CLIENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_RESOURCE_CLIENT_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class LayoutSVGResourceContainer;

typedef unsigned InvalidationModeMask;

class CORE_EXPORT SVGResourceClient : public GarbageCollectedMixin {
 public:
  virtual ~SVGResourceClient() = default;

  // When adding modes, make sure we don't overflow
  // |LayoutSVGResourceContainer::completed_invalidation_mask_|.
  enum InvalidationMode {
    kLayoutInvalidation = 1 << 0,
    kBoundariesInvalidation = 1 << 1,
    kPaintInvalidation = 1 << 2,
    kParentOnlyInvalidation = 1 << 3,
    kSkipAncestorInvalidation = 1 << 4,
  };
  virtual void ResourceContentChanged(InvalidationModeMask) = 0;
  virtual void ResourceElementChanged() = 0;
  virtual void ResourceDestroyed(LayoutSVGResourceContainer*) {}

 protected:
  SVGResourceClient() = default;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_RESOURCE_CLIENT_H_
