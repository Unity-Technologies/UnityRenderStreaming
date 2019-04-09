// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIASOURCE_TRACK_DEFAULT_LIST_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIASOURCE_TRACK_DEFAULT_LIST_H_

#include "third_party/blink/renderer/modules/mediasource/track_default.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class ExceptionState;

class TrackDefaultList final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static TrackDefaultList* Create();  // Creates an empty TrackDefaultList.

  // Implement the IDL
  static TrackDefaultList* Create(const HeapVector<Member<TrackDefault>>&,
                                  ExceptionState&);

  unsigned length() const { return track_defaults_.size(); }
  TrackDefault* item(unsigned) const;

  void Trace(blink::Visitor*) override;

 private:
  TrackDefaultList();

  explicit TrackDefaultList(const HeapVector<Member<TrackDefault>>&);

  const HeapVector<Member<TrackDefault>> track_defaults_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIASOURCE_TRACK_DEFAULT_LIST_H_
