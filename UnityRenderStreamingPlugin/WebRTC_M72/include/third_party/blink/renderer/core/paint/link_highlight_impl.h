/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_LINK_HIGHLIGHT_IMPL_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_LINK_HIGHLIGHT_IMPL_H_

#include <memory>

#include "cc/layers/content_layer_client.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/animation/compositor_animation.h"
#include "third_party/blink/renderer/platform/animation/compositor_animation_client.h"
#include "third_party/blink/renderer/platform/animation/compositor_animation_delegate.h"
#include "third_party/blink/renderer/platform/graphics/compositor_element_id.h"
#include "third_party/blink/renderer/platform/graphics/link_highlight.h"
#include "third_party/blink/renderer/platform/graphics/path.h"
#include "third_party/blink/renderer/platform/heap/persistent.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/time.h"

namespace cc {
class Layer;
class PictureLayer;
}

namespace blink {

class GraphicsLayer;
class LayoutBoxModelObject;
class Node;

class CORE_EXPORT LinkHighlightImpl final : public LinkHighlight,
                                            public cc::ContentLayerClient,
                                            public CompositorAnimationDelegate,
                                            public CompositorAnimationClient {
 public:
  static std::unique_ptr<LinkHighlightImpl> Create(Node*);
  ~LinkHighlightImpl() override;

  void StartHighlightAnimationIfNeeded();

  // Recalculates |path_| based on |node_|'s geometry and updates the link
  // highlight layer. To avoid re-computing |path_|, a dirty bit is used
  // (see |geometry_needs_update_| and |Invalidate()|) which is based on raster
  // invalidation of the owning graphics layer.
  void UpdateGeometry();

  // cc::ContentLayerClient implementation.
  gfx::Rect PaintableRegion() override;
  scoped_refptr<cc::DisplayItemList> PaintContentsToDisplayList(
      PaintingControlSetting painting_control) override;
  bool FillsBoundsCompletely() const override { return false; }
  size_t GetApproximateUnsharedMemoryUsage() const override { return 0; }

  // CompositorAnimationDelegate implementation.
  void NotifyAnimationStarted(double monotonic_time, int group) override;
  void NotifyAnimationFinished(double monotonic_time, int group) override;
  void NotifyAnimationAborted(double monotonic_time, int group) override {}

  // LinkHighlight implementation.
  void Invalidate() override;
  cc::Layer* Layer() override;
  void ClearCurrentGraphicsLayer() override;

  // CompositorAnimationClient implementation.
  CompositorAnimation* GetCompositorAnimation() const override;

  GraphicsLayer* CurrentGraphicsLayerForTesting() const {
    return current_graphics_layer_;
  }

  Node* GetNode() const { return node_; }

  CompositorElementId element_id();

  const EffectPaintPropertyNode* effect() override;

 private:
  LinkHighlightImpl(Node*);

  void ReleaseResources();
  void ComputeQuads(const Node&, Vector<FloatQuad>&) const;

  void AttachLinkHighlightToCompositingLayer(
      const LayoutBoxModelObject& paint_invalidation_container);
  void ClearGraphicsLayerLinkHighlightPointer();
  // This function computes the highlight path, and returns true if it has
  // changed size since the last call to this function.
  bool ComputeHighlightLayerPathAndPosition(const LayoutBoxModelObject&);

  scoped_refptr<cc::PictureLayer> content_layer_;
  Path path_;

  Persistent<Node> node_;
  GraphicsLayer* current_graphics_layer_;
  bool is_scrolling_graphics_layer_;
  std::unique_ptr<CompositorAnimation> compositor_animation_;

  bool geometry_needs_update_;
  bool is_animating_;
  TimeTicks start_time_;
  UniqueObjectId unique_id_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_LINK_HIGHLIGHT_IMPL_H_
