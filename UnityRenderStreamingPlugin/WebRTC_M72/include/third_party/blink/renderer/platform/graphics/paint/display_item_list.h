// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_PAINT_DISPLAY_ITEM_LIST_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_PAINT_DISPLAY_ITEM_LIST_H_

#include "third_party/blink/renderer/platform/graphics/contiguous_container.h"
#include "third_party/blink/renderer/platform/graphics/paint/display_item.h"
#include "third_party/blink/renderer/platform/graphics/paint/foreign_layer_display_item.h"
#include "third_party/blink/renderer/platform/wtf/alignment.h"
#include "third_party/blink/renderer/platform/wtf/assertions.h"

namespace blink {

class JSONArray;
struct PaintChunk;

// kDisplayItemAlignment must be a multiple of alignof(derived display item) for
// each derived display item; the ideal value is the least common multiple.
// The validity of kDisplayItemAlignment and kMaximumDisplayItemSize are checked
// in PaintController::CreateAndAppend().
static const size_t kDisplayItemAlignment = alignof(ForeignLayerDisplayItem);
static const size_t kMaximumDisplayItemSize = sizeof(ForeignLayerDisplayItem);

// A container for a list of display items.
class PLATFORM_EXPORT DisplayItemList
    : public ContiguousContainer<DisplayItem, kDisplayItemAlignment> {
 public:
  DisplayItemList(size_t initial_size_bytes)
      : ContiguousContainer(kMaximumDisplayItemSize, initial_size_bytes) {}
  DisplayItemList(DisplayItemList&& source)
      : ContiguousContainer(std::move(source)) {}

  DisplayItemList& operator=(DisplayItemList&& source) {
    ContiguousContainer::operator=(std::move(source));
    return *this;
  }

  DisplayItem& AppendByMoving(DisplayItem& item) {
    SECURITY_CHECK(!item.IsTombstone());
    DisplayItem& result =
        ContiguousContainer::AppendByMoving(item, item.DerivedSize());
    // ContiguousContainer::AppendByMoving() calls an in-place constructor
    // on item which replaces it with a tombstone/"dead display item" that
    // can be safely destructed but should never be used except for debugging.
    DCHECK(item.IsTombstone());
    DCHECK(item.GetId() == result.GetId());
    // We need |visual_rect_| and |outset_for_raster_effects_| of the old
    // display item for raster invalidation. As their values were initialized
    // to default values in DisplayItem's default constructor, now copy their
    // original values back from |result|.
    item.visual_rect_ = result.visual_rect_;
    item.outset_for_raster_effects_ = result.outset_for_raster_effects_;
    return result;
  }

  // Useful for iterating with a range-based for loop.
  template <typename Iterator>
  class Range {
   public:
    Range(const Iterator& begin, const Iterator& end)
        : begin_(begin), end_(end) {}
    Iterator begin() const { return begin_; }
    Iterator end() const { return end_; }

   private:
    Iterator begin_;
    Iterator end_;
  };
  Range<iterator> ItemsInPaintChunk(const PaintChunk&);
  Range<const_iterator> ItemsInPaintChunk(const PaintChunk&) const;

#if DCHECK_IS_ON()
  enum JsonOptions {
    kDefault = 0,
    kShowPaintRecords = 1,
    kSkipNonDrawings = 1 << 1,
    kClientKnownToBeAlive = 1 << 2,
    kShownOnlyDisplayItemTypes = 1 << 3
  };
  typedef unsigned JsonFlags;

  std::unique_ptr<JSONArray> SubsequenceAsJSON(size_t begin_index,
                                               size_t end_index,
                                               JsonFlags) const;
  void AppendSubsequenceAsJSON(size_t begin_index,
                               size_t end_index,
                               JsonFlags,
                               JSONArray&) const;
#endif  // DCHECK_IS_ON()
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_PAINT_DISPLAY_ITEM_LIST_H_
