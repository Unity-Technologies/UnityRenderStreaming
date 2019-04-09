// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_STYLE_ORDERED_NAMED_GRID_LINES_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_STYLE_ORDERED_NAMED_GRID_LINES_H_

#include "third_party/blink/renderer/platform/wtf/hash_map.h"
#include "third_party/blink/renderer/platform/wtf/hash_traits.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

using OrderedNamedGridLines =
    HashMap<size_t,
            Vector<String>,
            WTF::IntHash<size_t>,
            WTF::UnsignedWithZeroKeyHashTraits<size_t>>;

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_STYLE_ORDERED_NAMED_GRID_LINES_H_
