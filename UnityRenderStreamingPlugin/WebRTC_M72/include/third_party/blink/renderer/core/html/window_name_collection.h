// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_WINDOW_NAME_COLLECTION_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_WINDOW_NAME_COLLECTION_H_

#include "third_party/blink/renderer/core/html/html_name_collection.h"

namespace blink {

class WindowNameCollection final : public HTMLNameCollection {
 public:
  static WindowNameCollection* Create(ContainerNode& document,
                                      CollectionType type,
                                      const AtomicString& name) {
    DCHECK_EQ(type, kWindowNamedItems);
    return new WindowNameCollection(document, name);
  }

  bool ElementMatches(const Element&) const;

 private:
  WindowNameCollection(ContainerNode& document, const AtomicString& name);
};

DEFINE_TYPE_CASTS(WindowNameCollection,
                  LiveNodeListBase,
                  collection,
                  collection->GetType() == kWindowNamedItems,
                  collection.GetType() == kWindowNamedItems);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_WINDOW_NAME_COLLECTION_H_
