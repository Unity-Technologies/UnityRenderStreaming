// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_LAYOUT_OBJECT_COUNTER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_LAYOUT_OBJECT_COUNTER_H_

namespace blink {

// Used by LocalFrameView and FirstMeaningfulPaintDetector to keep track of
// the number of layout objects created in the frame.
class LayoutObjectCounter {
 public:
  void Reset() { count_ = 0; }
  void Increment() { count_++; }
  unsigned Count() const { return count_; }

 private:
  unsigned count_ = 0;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_FIRST_MEANINGFUL_PAINT_DETECTOR_H_
