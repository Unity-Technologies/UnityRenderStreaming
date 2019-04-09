// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_KEYBOARD_KEYBOARD_LAYOUT_MAP_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_KEYBOARD_KEYBOARD_LAYOUT_MAP_H_

#include "third_party/blink/renderer/bindings/core/v8/maplike.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_binding_for_core.h"
#include "third_party/blink/renderer/platform/bindings/exception_state.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class KeyboardLayoutMap final : public ScriptWrappable,
                                public Maplike<String, String> {
  DEFINE_WRAPPERTYPEINFO();

 public:
  KeyboardLayoutMap(const HashMap<String, String>& map);

  const HashMap<String, String>& Map() const { return layout_map_; }

  // IDL attributes / methods
  uint32_t size() const { return layout_map_.size(); }

  void Trace(blink::Visitor* visitor) override {
    ScriptWrappable::Trace(visitor);
  }

 private:
  // Maplike implementation.
  PairIterable<String, String>::IterationSource* StartIteration(
      ScriptState*,
      ExceptionState&) override;
  bool GetMapEntry(ScriptState*,
                   const String& key,
                   String& value,
                   ExceptionState&) override;

  const HashMap<String, String> layout_map_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_KEYBOARD_KEYBOARD_LAYOUT_MAP_H_
