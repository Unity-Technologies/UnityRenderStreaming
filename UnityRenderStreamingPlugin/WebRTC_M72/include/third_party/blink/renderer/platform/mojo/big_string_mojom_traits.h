// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_MOJO_BIG_STRING_MOJOM_TRAITS_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_MOJO_BIG_STRING_MOJOM_TRAITS_H_

#include "mojo/public/cpp/bindings/struct_traits.h"
#include "mojo/public/mojom/base/big_string.mojom-blink.h"
#include "third_party/blink/renderer/platform/platform_export.h"

namespace mojo_base {
class BigBuffer;
}

namespace mojo {

template <>
struct PLATFORM_EXPORT
    StructTraits<mojo_base::mojom::BigStringDataView, WTF::String> {
  static bool IsNull(const WTF::String& input) { return input.IsNull(); }
  static void SetToNull(WTF::String* output) { *output = WTF::String(); }

  static mojo_base::BigBuffer data(const WTF::String& input);
  static bool Read(mojo_base::mojom::BigStringDataView, WTF::String* out);
};

}  // namespace mojo

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_MOJO_BIG_STRING_MOJOM_TRAITS_H_
