// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_SURFACE_LAYER_BRIDGE_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_SURFACE_LAYER_BRIDGE_H_

#include <memory>

#include "base/time/time.h"
#include "cc/layers/surface_layer.h"
#include "components/viz/common/surfaces/surface_id.h"
#include "third_party/blink/public/platform/web_common.h"
#include "third_party/blink/public/platform/web_layer_tree_view.h"

namespace blink {

// Listens for updates made on the cc::Layer by the WebSurfaceLayerBridge.
class BLINK_PLATFORM_EXPORT WebSurfaceLayerBridgeObserver {
 public:
  // Triggered by resizing or surface layer creation.
  virtual void OnWebLayerUpdated() = 0;

  // Called when new a SurfaceLayer is created.
  virtual void RegisterContentsLayer(cc::Layer*) = 0;
  virtual void UnregisterContentsLayer(cc::Layer*) = 0;

  // Called when a SurfaceLayer is activated.
  virtual void OnSurfaceIdUpdated(viz::SurfaceId surface_id) {}
};

// Maintains and exposes the SurfaceLayer.
class BLINK_PLATFORM_EXPORT WebSurfaceLayerBridge {
 public:
  static std::unique_ptr<WebSurfaceLayerBridge> Create(
      WebLayerTreeView*,
      WebSurfaceLayerBridgeObserver*,
      cc::UpdateSubmissionStateCB);
  virtual ~WebSurfaceLayerBridge();
  virtual cc::Layer* GetCcLayer() const = 0;
  virtual const viz::FrameSinkId& GetFrameSinkId() const = 0;
  virtual const viz::SurfaceId& GetSurfaceId() const = 0;
  virtual base::TimeTicks GetLocalSurfaceIdAllocationTime() const = 0;
  virtual void ClearSurfaceId() = 0;
  virtual void SetContentsOpaque(bool) = 0;
  virtual void CreateSurfaceLayer() = 0;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_SURFACE_LAYER_BRIDGE_H_
