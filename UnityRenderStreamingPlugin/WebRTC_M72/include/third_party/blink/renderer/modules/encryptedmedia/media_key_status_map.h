// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_ENCRYPTEDMEDIA_MEDIA_KEY_STATUS_MAP_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_ENCRYPTEDMEDIA_MEDIA_KEY_STATUS_MAP_H_

#include "third_party/blink/renderer/bindings/core/v8/iterable.h"
#include "third_party/blink/renderer/bindings/core/v8/script_value.h"
#include "third_party/blink/renderer/core/typed_arrays/dom_array_piece.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"

namespace blink {

class ExceptionState;
class ScriptState;
class WebData;

// Represents a read-only map (to JavaScript) of key IDs and their current
// status known to a particular session. Since it can be updated any time there
// is a keychange event, iteration order and completeness is not guaranteed
// if the event loop runs.
class MediaKeyStatusMap final
    : public ScriptWrappable,
      public PairIterable<ArrayBufferOrArrayBufferView, String> {
  DEFINE_WRAPPERTYPEINFO();

 private:
  // MapEntry holds the keyId (DOMArrayBuffer) and status (MediaKeyStatus as
  // String) for each entry.
  class MapEntry;

  // The key ids and their status are kept in a list, as order is important.
  // Note that order (or lack of it) is not specified in the EME spec.
  using MediaKeyStatusMapType = HeapVector<Member<MapEntry>>;

 public:
  MediaKeyStatusMap() = default;

  void Clear();
  void AddEntry(WebData key_id, const String& status);
  const MapEntry& at(uint32_t) const;

  // IDL attributes / methods
  uint32_t size() const { return entries_.size(); }
  bool has(const ArrayBufferOrArrayBufferView& key_id);
  ScriptValue get(ScriptState*, const ArrayBufferOrArrayBufferView& key_id);

  void Trace(blink::Visitor*) override;

 private:
  // PairIterable<> implementation.
  IterationSource* StartIteration(ScriptState*, ExceptionState&) override;

  uint32_t IndexOf(const DOMArrayPiece& key_id) const;

  MediaKeyStatusMapType entries_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_ENCRYPTEDMEDIA_MEDIA_KEY_STATUS_MAP_H_
