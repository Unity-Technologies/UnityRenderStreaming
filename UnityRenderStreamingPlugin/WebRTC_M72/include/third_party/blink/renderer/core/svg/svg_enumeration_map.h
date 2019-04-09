// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_ENUMERATION_MAP_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_ENUMERATION_MAP_H_

#include "third_party/blink/renderer/platform/wtf/assertions.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

namespace blink {

// Helper class for SVG enumerations. Maps between name (string) and value.
//
// It is assumed that enumeration values are contiguous, non-zero and
// starting at 1.
//
// For enumerations that have had new values added since SVG 1.1, the
// |max_exposed_value| should be set to the last old value. From this also
// follow that the new values should sort last - after the |max_exposed_value|.
// (This is currently always the case in the spec too.)
class SVGEnumerationMap {
 public:
  struct Entry {
    const unsigned short value;
    const char* const name;
  };

  template <unsigned entries_length>
  constexpr SVGEnumerationMap(const Entry (&entries)[entries_length])
      : SVGEnumerationMap(entries, entries[entries_length - 1].value) {}
  template <unsigned entries_length>
  constexpr SVGEnumerationMap(const Entry (&entries)[entries_length],
                              unsigned short max_exposed_value)
      : entries_(entries),
        num_entries_(entries_length),
        max_exposed_value_(max_exposed_value) {}

  const char* NameFromValue(unsigned short value) const {
    DCHECK(value);  // We should never store 0 (*_UNKNOWN) in the map.
    DCHECK_LT(value - 1, num_entries_);
    DCHECK_EQ(entries_[value - 1].value, value);
    return entries_[value - 1].name;
  }
  unsigned short ValueFromName(const String&) const;

  unsigned short ValueOfLast() const {
    return entries_[num_entries_ - 1].value;
  }
  unsigned short MaxExposedValue() const { return max_exposed_value_; }

 private:
  const Entry* begin() const { return entries_; }
  const Entry* end() const { return entries_ + num_entries_; }

  const Entry* const entries_;
  const unsigned short num_entries_;
  const unsigned short max_exposed_value_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_ENUMERATION_MAP_H_
