/*
 * Copyright (C) 2013 Google Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_PARSER_HTML_RESOURCE_PRELOADER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_PARSER_HTML_RESOURCE_PRELOADER_H_

#include <memory>

#include "base/macros.h"
#include "third_party/blink/renderer/core/html/parser/preload_request.h"
#include "third_party/blink/renderer/core/html/parser/resource_preloader.h"
#include "third_party/blink/renderer/core/loader/network_hints_interface.h"
#include "third_party/blink/renderer/platform/heap/heap.h"
#include "third_party/blink/renderer/platform/wtf/text/text_position.h"
#include "third_party/blink/renderer/platform/wtf/time.h"

namespace blink {

class Document;

class CORE_EXPORT HTMLResourcePreloader
    : public GarbageCollected<HTMLResourcePreloader>,
      public ResourcePreloader {
  friend class HTMLResourcePreloaderTest;

 public:
  static HTMLResourcePreloader* Create(Document&);

  explicit HTMLResourcePreloader(Document&);

  int CountPreloads();
  Document* GetDocument() { return document_.Get(); }
  void Trace(blink::Visitor*);

 protected:
  void Preload(std::unique_ptr<PreloadRequest>,
               const NetworkHintsInterface&) override;

 private:
  Member<Document> document_;

  DISALLOW_COPY_AND_ASSIGN(HTMLResourcePreloader);
};

}  // namespace blink

#endif
