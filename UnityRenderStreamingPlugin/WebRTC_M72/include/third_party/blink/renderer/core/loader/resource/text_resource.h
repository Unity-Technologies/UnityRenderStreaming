// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_RESOURCE_TEXT_RESOURCE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_RESOURCE_TEXT_RESOURCE_H_

#include <memory>
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/html/parser/text_resource_decoder.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource.h"
#include "third_party/blink/renderer/platform/loader/fetch/text_resource_decoder_options.h"

namespace blink {

class CORE_EXPORT TextResource : public Resource {
 public:
  // Returns the decoded data in text form. The data has to be available at
  // call time.
  String DecodedText() const;

  WTF::TextEncoding Encoding() const override;

  void SetEncodingForTest(const String& encoding) { SetEncoding(encoding); }

 protected:
  TextResource(const ResourceRequest&,
               ResourceType,
               const ResourceLoaderOptions&,
               const TextResourceDecoderOptions&);
  ~TextResource() override;

  void SetEncoding(const String&) override;

 private:
  std::unique_ptr<TextResourceDecoder> decoder_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_RESOURCE_TEXT_RESOURCE_H_
