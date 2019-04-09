// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_CSSPAINT_CSS_PAINT_WORKLET_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_CSSPAINT_CSS_PAINT_WORKLET_H_

#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class ScriptState;
class Worklet;

class MODULES_EXPORT CSSPaintWorklet {
  STATIC_ONLY(CSSPaintWorklet);

 public:
  static Worklet* paintWorklet(ScriptState*);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_CSSPAINT_CSS_PAINT_WORKLET_H_
