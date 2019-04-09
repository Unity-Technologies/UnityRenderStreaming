// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_I18N_BIDI_LINE_ITERATOR_H_
#define BASE_I18N_BIDI_LINE_ITERATOR_H_

#include "base/i18n/base_i18n_export.h"
#include "base/i18n/rtl.h"
#include "base/macros.h"
#include "base/strings/string16.h"
#include "third_party/icu/source/common/unicode/ubidi.h"
#include "third_party/icu/source/common/unicode/uchar.h"

namespace base {
namespace i18n {

// A simple wrapper class for the bidirectional iterator of ICU.
// This class uses the bidirectional iterator of ICU to split a line of
// bidirectional texts into visual runs in its display order.
class BASE_I18N_EXPORT BiDiLineIterator {
 public:
  // Specifies some alternative iteration behavior.
  enum class CustomBehavior {
    // No special behavior.
    NONE,
    // Treat URL delimiter characters as strong LTR. This is a special treatment
    // for URLs that purposefully violates the URL Standard, as an experiment.
    // It should only be used behind a flag.
    AS_URL
  };

  BiDiLineIterator();
  ~BiDiLineIterator();

  // Initializes the bidirectional iterator with the specified text.  Returns
  // whether initialization succeeded.
  bool Open(const string16& text,
            TextDirection direction,
            CustomBehavior behavior);

  // Returns the number of visual runs in the text, or zero on error.
  int CountRuns() const;

  // Gets the logical offset, length, and direction of the specified visual run.
  UBiDiDirection GetVisualRun(int index, int* start, int* length) const;

  // Given a start position, figure out where the run ends (and the BiDiLevel).
  void GetLogicalRun(int start, int* end, UBiDiLevel* level) const;

 private:
  UBiDi* bidi_;

  DISALLOW_COPY_AND_ASSIGN(BiDiLineIterator);
};

}  // namespace i18n
}  // namespace base

#endif  // BASE_I18N_BIDI_LINE_ITERATOR_H_
