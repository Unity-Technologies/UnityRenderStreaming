// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_CANVAS_HTMLCANVAS_CANVAS_CONTEXT_CREATION_ATTRIBUTES_HELPERS_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_CANVAS_HTMLCANVAS_CANVAS_CONTEXT_CREATION_ATTRIBUTES_HELPERS_H_

namespace blink {

class CanvasContextCreationAttributesCore;
class CanvasContextCreationAttributesModule;

CanvasContextCreationAttributesCore ToCanvasContextCreationAttributes(
    const CanvasContextCreationAttributesModule*);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_CANVAS_HTMLCANVAS_CANVAS_CONTEXT_CREATION_ATTRIBUTES_HELPERS_H_
