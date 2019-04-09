// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_MARKERS_ACTIVE_SUGGESTION_MARKER_LIST_IMPL_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_MARKERS_ACTIVE_SUGGESTION_MARKER_LIST_IMPL_H_

#include "third_party/blink/renderer/core/editing/markers/document_marker_list.h"

namespace blink {

// Implementation of DocumentMarkerList for ActiveSuggestion markers.
// ActiveSuggestion markers are always inserted in order, aside from some
// potential oddball cases (e.g. splitting the marker list into two nodes, then
// undoing the split). This means we can keep the list in sorted order to do
// some operations more efficiently, while still being able to do inserts in
// O(1) time at the end of the list.
class CORE_EXPORT ActiveSuggestionMarkerListImpl final
    : public DocumentMarkerList {
 public:
  ActiveSuggestionMarkerListImpl() = default;

  // DocumentMarkerList implementations
  DocumentMarker::MarkerType MarkerType() const final;

  bool IsEmpty() const final;

  void Add(DocumentMarker*) final;
  void Clear() final;

  const HeapVector<Member<DocumentMarker>>& GetMarkers() const final;
  DocumentMarker* FirstMarkerIntersectingRange(unsigned start_offset,
                                               unsigned end_offset) const final;
  HeapVector<Member<DocumentMarker>> MarkersIntersectingRange(
      unsigned start_offset,
      unsigned end_offset) const final;

  bool MoveMarkers(int length, DocumentMarkerList* dst_list) final;
  bool RemoveMarkers(unsigned start_offset, int length) final;
  bool ShiftMarkers(const String& node_text,
                    unsigned offset,
                    unsigned old_length,
                    unsigned new_length) final;

  void Trace(blink::Visitor*) override;

 private:
  HeapVector<Member<DocumentMarker>> markers_;

  DISALLOW_COPY_AND_ASSIGN(ActiveSuggestionMarkerListImpl);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_MARKERS_ACTIVE_SUGGESTION_MARKER_LIST_IMPL_H_
