// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_DOCUMENT_MARKER_PAINTER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_DOCUMENT_MARKER_PAINTER_H_

#include "third_party/blink/renderer/core/editing/markers/document_marker.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace blink {

class ComputedStyle;
class FloatRect;
class GraphicsContext;
class LayoutPoint;
class LayoutRect;
class LayoutUnit;
class StyleableMarker;
class TextMatchMarker;
struct TextPaintStyle;

// Document marker painter for both LayoutNG and legacy layout.
// This paints text decorations for spell/grammer check, find-in-page, and
// input method.
class DocumentMarkerPainter {
  STATIC_ONLY(DocumentMarkerPainter);

 public:
  static void PaintStyleableMarkerUnderline(GraphicsContext& context,
                                            const LayoutPoint& box_origin,
                                            const StyleableMarker& marker,
                                            const ComputedStyle& style,
                                            const FloatRect& marker_rect,
                                            LayoutUnit logical_height);
  static void PaintDocumentMarker(GraphicsContext& context,
                                  const LayoutPoint& box_origin,
                                  const ComputedStyle& style,
                                  DocumentMarker::MarkerType marker_type,
                                  const LayoutRect& local_rect);
  static TextPaintStyle ComputeTextPaintStyleFrom(
      const ComputedStyle& style,
      const TextMatchMarker& marker);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_DOCUMENT_MARKER_PAINTER_H_
