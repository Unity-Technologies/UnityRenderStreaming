// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_MARKERS_SUGGESTION_MARKER_LIST_IMPL_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_MARKERS_SUGGESTION_MARKER_LIST_IMPL_H_

#include "third_party/blink/renderer/core/editing/markers/document_marker_list.h"

namespace blink {

// Implementation of DocumentMarkerList for Suggestion markers. Suggestion
// markers are somewhat unusual compared to some other MarkerTypes in that they
// can overlap. Since sorting by start offset doesn't help very much when the
// markers can overlap, and other ways of doing this efficiently would add a lot
// of complexity, suggestion markers are currently stored unsorted.
class CORE_EXPORT SuggestionMarkerListImpl final : public DocumentMarkerList {
 public:
  SuggestionMarkerListImpl() = default;

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

  // SuggestionMarkerListImpl-specific
  bool RemoveMarkerByTag(int32_t tag);

 private:
  bool ShiftMarkersForSuggestionReplacement(unsigned offset,
                                            unsigned old_length,
                                            unsigned new_length);
  bool ShiftMarkersForNonSuggestionEditingOperation(const String& node_text,
                                                    unsigned offset,
                                                    unsigned old_length,
                                                    unsigned new_length);

  HeapVector<Member<DocumentMarker>> markers_;

  DISALLOW_COPY_AND_ASSIGN(SuggestionMarkerListImpl);
};

DEFINE_TYPE_CASTS(SuggestionMarkerListImpl,
                  DocumentMarkerList,
                  list,
                  list->MarkerType() == DocumentMarker::kSuggestion,
                  list.MarkerType() == DocumentMarker::kSuggestion);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_MARKERS_SUGGESTION_MARKER_LIST_IMPL_H_
