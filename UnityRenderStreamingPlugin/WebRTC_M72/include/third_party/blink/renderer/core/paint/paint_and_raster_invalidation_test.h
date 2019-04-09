// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_PAINT_AND_RASTER_INVALIDATION_TEST_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_PAINT_AND_RASTER_INVALIDATION_TEST_H_

#include "third_party/blink/renderer/core/paint/paint_controller_paint_test.h"
#include "third_party/blink/renderer/platform/graphics/compositing/content_layer_client_impl.h"
#include "third_party/blink/renderer/platform/graphics/compositing/paint_artifact_compositor.h"
#include "third_party/blink/renderer/platform/testing/layer_tree_host_embedder.h"

namespace blink {

class PaintAndRasterInvalidationTest : public PaintControllerPaintTest {
 public:
  PaintAndRasterInvalidationTest()
      : PaintControllerPaintTest(SingleChildLocalFrameClient::Create()) {}

 protected:
  ContentLayerClientImpl* GetContentLayerClient(size_t index = 0) const {
    DCHECK(RuntimeEnabledFeatures::SlimmingPaintV2Enabled());
    const auto& clients = GetDocument()
                              .View()
                              ->GetPaintArtifactCompositorForTesting()
                              ->ContentLayerClientsForTesting();
    return index < clients.size() ? clients[index].get() : nullptr;
  }

  const RasterInvalidationTracking* GetRasterInvalidationTracking(
      size_t index = 0) const {
    if (RuntimeEnabledFeatures::SlimmingPaintV2Enabled()) {
      if (auto* client = GetContentLayerClient(index))
        return client->GetRasterInvalidator().GetTracking();
      return nullptr;
    }
    return GetLayoutView()
        .Layer()
        ->GraphicsLayerBacking()
        ->GetRasterInvalidationTracking();
  }

  void SetUp() override {
    PaintControllerPaintTest::SetUp();

    if (RuntimeEnabledFeatures::SlimmingPaintV2Enabled()) {
      layer_tree_ = std::make_unique<LayerTreeHostEmbedder>();
      layer_tree_->layer_tree_host()->SetRootLayer(
          GetDocument()
              .View()
              ->GetPaintArtifactCompositorForTesting()
              ->RootLayer());
    }
  }

  void SetPreferCompositingToLCDText(bool enable) {
    GetDocument()
        .GetFrame()
        ->GetSettings()
        ->SetPreferCompositingToLCDTextEnabled(enable);
  }

 private:
  std::unique_ptr<LayerTreeHostEmbedder> layer_tree_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_PAINT_AND_RASTER_INVALIDATION_TEST_H_
