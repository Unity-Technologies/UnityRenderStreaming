// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_ACCESSIBILITY_AX_CONTEXT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_ACCESSIBILITY_AX_CONTEXT_H_

#include "base/macros.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/platform/heap/persistent.h"

namespace blink {

class AXObjectCache;

// An AXContext enables accessibility support in a Document for as
// long as the AXContext is alive. While the AXContext exists,
// Document::ExistingAXObjectCache will always return a valid
// AXObjectCache if the document is still active.
class CORE_EXPORT AXContext {
 public:
  explicit AXContext(Document& document);
  virtual ~AXContext();

  // Note: it's an error to call this after |document| is no longer active.
  // The caller should check this.
  AXObjectCache& GetAXObjectCache();

 protected:
  WeakPersistent<Document> document_;

  DISALLOW_COPY_AND_ASSIGN(AXContext);
};

}  // namespace blink

#endif
