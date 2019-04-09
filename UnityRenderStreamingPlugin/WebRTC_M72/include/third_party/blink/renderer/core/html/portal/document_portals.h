// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_PORTAL_DOCUMENT_PORTALS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_PORTAL_DOCUMENT_PORTALS_H_

#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/supplementable.h"

namespace blink {

class HTMLPortalElement;

class DocumentPortals : public GarbageCollected<DocumentPortals>,
                        public Supplement<Document> {
  USING_GARBAGE_COLLECTED_MIXIN(DocumentPortals);

 public:
  static const char kSupplementName[];
  static DocumentPortals& From(Document&);

  // Used to notify the document that a portal was inserted.
  void OnPortalInserted(HTMLPortalElement* portal);

  // Notifies that a portal was removed from the Document.
  void OnPortalRemoved(HTMLPortalElement* portal);

  // Retrieves the portal identified by the token.
  HTMLPortalElement* GetPortal(const base::UnguessableToken&) const;

  void Trace(Visitor*) override;

 private:
  explicit DocumentPortals(Document&);

  HeapVector<Member<HTMLPortalElement>> portals_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_PORTAL_DOCUMENT_PORTALS_H_
