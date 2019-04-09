// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_ALLOWED_BY_NOSNIFF_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_ALLOWED_BY_NOSNIFF_H_

#include "third_party/blink/renderer/core/core_export.h"

namespace blink {

class ExecutionContext;
class ResourceResponse;

class CORE_EXPORT AllowedByNosniff {
 public:
  enum class MimeTypeCheck { kStrict, kLax };

  static bool MimeTypeAsScript(ExecutionContext*,
                               const ResourceResponse&,
                               MimeTypeCheck mime_type_check_mode);
};

}  // namespace blink

#endif
