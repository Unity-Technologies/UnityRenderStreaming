// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_XR_XR_FRAME_PROVIDER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_XR_XR_FRAME_PROVIDER_H_

#include "device/vr/public/mojom/vr_service.mojom-blink.h"
#include "mojo/public/cpp/bindings/binding.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/geometry/int_size.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

namespace blink {

class XRDevice;
class XRSession;
class XRFrameTransport;
class XRWebGLLayer;

// This class manages requesting and dispatching frame updates, which includes
// pose information for a given XRDevice.
class XRFrameProvider final
    : public GarbageCollectedFinalized<XRFrameProvider> {
 public:
  explicit XRFrameProvider(XRDevice*);

  XRSession* immersive_session() const { return immersive_session_; }
  device::mojom::blink::XRPresentationClientPtr GetSubmitFrameClient();

  void BeginImmersiveSession(XRSession* session,
                             device::mojom::blink::XRSessionPtr session_ptr);
  void OnImmersiveSessionEnded();

  void RequestFrame(XRSession*);

  void OnNonImmersiveVSync(double high_res_now_ms);

  void SubmitWebGLLayer(XRWebGLLayer*, bool was_changed);
  void UpdateWebGLLayerViewports(XRWebGLLayer*);

  void Dispose();
  void OnFocusChanged();

  virtual void Trace(blink::Visitor*);

 private:
  void OnImmersiveFrameData(device::mojom::blink::XRFrameDataPtr data);
  void OnNonImmersiveFrameData(device::mojom::blink::XRFrameDataPtr data);

  void ScheduleImmersiveFrame();
  void ScheduleNonImmersiveFrame();

  void OnPresentationProviderConnectionError();
  void ProcessScheduledFrame(device::mojom::blink::XRFrameDataPtr frame_data,
                             double high_res_now_ms);

  bool HasARSession();

  const Member<XRDevice> device_;
  Member<XRSession> immersive_session_;
  Member<XRFrameTransport> frame_transport_;

  // Non-immersive Sessions which have requested a frame update.
  HeapVector<Member<XRSession>> requesting_sessions_;
  HeapVector<Member<XRSession>> processing_sessions_;

  device::mojom::blink::XRPresentationProviderPtr presentation_provider_;
  device::mojom::blink::XRFrameDataProviderPtr immersive_data_provider_;
  device::mojom::blink::VRPosePtr frame_pose_;

  // This frame ID is XR-specific and is used to track when frames arrive at the
  // XR compositor so that it knows which poses to use, when to apply bounds
  // updates, etc.
  int16_t frame_id_ = -1;
  bool pending_immersive_vsync_ = false;
  bool pending_non_immersive_vsync_ = false;
  bool vsync_connection_failed_ = false;

  base::Optional<gpu::MailboxHolder> buffer_mailbox_holder_;
  bool last_has_focus_ = false;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_XR_XR_FRAME_PROVIDER_H_
