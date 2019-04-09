// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_METRICS_UKM_SOURCE_ID_H_
#define BASE_METRICS_UKM_SOURCE_ID_H_

#include <stdint.h>

#include "base/base_export.h"

namespace base {

// An ID used to identify a Source to UKM, for recording information about it.
// These objects are copyable, assignable, and occupy 64-bits per instance.
// Prefer passing them by value.
class BASE_EXPORT UkmSourceId {
 public:
  enum class Type : int64_t {
    UKM = 0,
    NAVIGATION_ID = 1,
    APP_ID = 2,
  };

  // Default constructor has the invalid value.
  constexpr UkmSourceId() : value_(0) {}

  constexpr UkmSourceId& operator=(UkmSourceId other) {
    value_ = other.value_;
    return *this;
  }

  // Allow identity comparisons.
  constexpr bool operator==(UkmSourceId other) const {
    return value_ == other.value_;
  }
  constexpr bool operator!=(UkmSourceId other) const {
    return value_ != other.value_;
  }

  // Allow coercive comparisons to simplify test migration.
  // TODO(crbug/873866): Remove these once callers are migrated.
  constexpr bool operator==(int64_t other) const { return value_ == other; }
  constexpr bool operator!=(int64_t other) const { return value_ == other; }

  // Extract the Type of the SourceId.
  Type GetType() const;

  // Return the ID as an int64.
  constexpr int64_t ToInt64() const { return value_; }

  // Convert an int64 ID value to an ID.
  static constexpr UkmSourceId FromInt64(int64_t internal_value) {
    return UkmSourceId(internal_value);
  }

  // Get a new UKM-Type SourceId, which is unique within the scope of a
  // browser session.
  static UkmSourceId New();

  // Utility for converting other unique ids to source ids.
  static UkmSourceId FromOtherId(int64_t value, Type type);

 private:
  constexpr explicit UkmSourceId(int64_t value) : value_(value) {}
  int64_t value_;
};

constexpr UkmSourceId kInvalidUkmSourceId = UkmSourceId();

}  // namespace base

#endif  // BASE_METRICS_UKM_SOURCE_ID_H_
