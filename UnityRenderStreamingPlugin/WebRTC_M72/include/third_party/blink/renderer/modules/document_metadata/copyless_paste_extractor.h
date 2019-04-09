// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_DOCUMENT_METADATA_COPYLESS_PASTE_EXTRACTOR_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_DOCUMENT_METADATA_COPYLESS_PASTE_EXTRACTOR_H_

#include "third_party/blink/public/platform/modules/document_metadata/copyless_paste.mojom-blink.h"
#include "third_party/blink/renderer/modules/modules_export.h"

namespace blink {

class Document;

// Extract structured metadata (currently schema.org in JSON-LD) for the
// Copyless Paste feature. The extraction must be done after the document
// has finished parsing.
class MODULES_EXPORT CopylessPasteExtractor final {
 public:
  static mojom::document_metadata::blink::WebPagePtr extract(const Document&);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_DOCUMENT_METADATA_COPYLESS_PASTE_EXTRACTOR_H_
