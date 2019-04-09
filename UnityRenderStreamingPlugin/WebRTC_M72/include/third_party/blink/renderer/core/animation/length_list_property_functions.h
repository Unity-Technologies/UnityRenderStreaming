// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_LENGTH_LIST_PROPERTY_FUNCTIONS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_LENGTH_LIST_PROPERTY_FUNCTIONS_H_

#include "third_party/blink/renderer/core/css/css_property_names.h"
#include "third_party/blink/renderer/platform/geometry/length.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class ComputedStyle;
class CSSProperty;

class LengthListPropertyFunctions {
  STATIC_ONLY(LengthListPropertyFunctions);

 public:
  static ValueRange GetValueRange(const CSSProperty&);
  static bool GetInitialLengthList(const CSSProperty&, Vector<Length>& result);
  static bool GetLengthList(const CSSProperty&,
                            const ComputedStyle&,
                            Vector<Length>& result);
  static void SetLengthList(const CSSProperty&,
                            ComputedStyle&,
                            Vector<Length>&& length_list);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_ANIMATION_LENGTH_LIST_PROPERTY_FUNCTIONS_H_
