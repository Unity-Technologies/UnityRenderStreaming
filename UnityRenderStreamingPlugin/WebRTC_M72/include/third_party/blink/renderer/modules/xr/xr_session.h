// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_XR_XR_SESSION_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_XR_XR_SESSION_H_

#include "device/vr/public/mojom/vr_service.mojom-blink.h"
#include "mojo/public/cpp/bindings/binding.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise.h"
#include "third_party/blink/renderer/core/dom/events/event_target.h"
#include "third_party/blink/renderer/core/typed_arrays/array_buffer_view_helpers.h"
#include "third_party/blink/renderer/core/typed_arrays/dom_typed_array.h"
#include "third_party/blink/renderer/modules/xr/xr_frame_request_callback_collection.h"
#include "third_party/blink/renderer/modules/xr/xr_input_source.h"
#include "third_party/blink/renderer/platform/bindings/trace_wrapper_member.h"
#include "third_party/blink/renderer/platform/geometry/double_size.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/transforms/transformation_matrix.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

#include "third_party/blink/renderer/bindings/core/v8/active_script_wrappable.h"

namespace blink {

class Element;
class ResizeObserver;
class ScriptPromiseResolver;
class V8XRFrameRequestCallback;
class XRCanvasInputProvider;
class XRCoordinateSystem;
class XRDevice;
class XRFrameOfReferenceOptions;
class XRInputSourceEvent;
class XRLayer;
class XRPresentationContext;
class XRView;

class XRSession final : public EventTargetWithInlineData,
                        public device::mojom::blink::XRSessionClient,
                        public ActiveScriptWrappable<XRSession> {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(XRSession);

 public:
  enum EnvironmentBlendMode {
    kBlendModeOpaque = 1,
    kBlendModeAdditive = 2,
    kBlendModeAlphaBlend = 3
  };

  XRSession(XRDevice*,
            device::mojom::blink::XRSessionClientRequest client_request,
            bool immersive,
            bool environment_integration,
            XRPresentationContext* output_context,
            EnvironmentBlendMode environment_blend_mode);
  ~XRSession() override = default;

  XRDevice* device() const { return device_; }
  bool immersive() const { return immersive_; }
  bool environmentIntegration() const { return environment_integration_; }
  XRPresentationContext* outputContext() const { return output_context_; }
  const String& environmentBlendMode() const { return blend_mode_string_; }

  // Near and far depths are used when computing projection matrices for this
  // Session's views. Changes will propegate to the appropriate matrices on the
  // next frame after these values are updated.
  double depthNear() const { return depth_near_; }
  void setDepthNear(double value);
  double depthFar() const { return depth_far_; }
  void setDepthFar(double value);

  XRLayer* baseLayer() const { return base_layer_; }
  void setBaseLayer(XRLayer* value);

  DEFINE_ATTRIBUTE_EVENT_LISTENER(blur, kBlur);
  DEFINE_ATTRIBUTE_EVENT_LISTENER(focus, kFocus);
  DEFINE_ATTRIBUTE_EVENT_LISTENER(resetpose, kResetpose);
  DEFINE_ATTRIBUTE_EVENT_LISTENER(end, kEnd);

  DEFINE_ATTRIBUTE_EVENT_LISTENER(selectstart, kSelectstart);
  DEFINE_ATTRIBUTE_EVENT_LISTENER(selectend, kSelectend);
  DEFINE_ATTRIBUTE_EVENT_LISTENER(select, kSelect);

  ScriptPromise requestFrameOfReference(ScriptState*,
                                        const String& type,
                                        const XRFrameOfReferenceOptions*);

  int requestAnimationFrame(V8XRFrameRequestCallback*);
  void cancelAnimationFrame(int id);

  using InputSourceMap =
      HeapHashMap<uint32_t, TraceWrapperMember<XRInputSource>>;

  HeapVector<Member<XRInputSource>> getInputSources() const;

  ScriptPromise requestHitTest(ScriptState* script_state,
                               NotShared<DOMFloat32Array> origin,
                               NotShared<DOMFloat32Array> direction,
                               XRCoordinateSystem* coordinate_system);

  // Called by JavaScript to manually end the session.
  ScriptPromise end(ScriptState*);

  bool ended() { return ended_; }

  // Called when the session is ended, either via calling the "end" function or
  // when the presentation service connection is closed.
  void ForceEnd();

  // Describes the scalar to be applied to the default framebuffer dimensions
  // which gives 1:1 pixel ratio at the center of the user's view.
  double NativeFramebufferScale() const;

  // Describes the recommended dimensions of layer framebuffers. Should be a
  // value that provides a good balance between quality and performance.
  DoubleSize DefaultFramebufferSize() const;

  // Reports the size of the output context's, if one is available. If not
  // reports (0, 0);
  DoubleSize OutputCanvasSize() const;

  void LogGetPose() const;

  // EventTarget overrides.
  ExecutionContext* GetExecutionContext() const override;
  const AtomicString& InterfaceName() const override;

  void OnFocusChanged();
  void OnFrame(double timestamp,
               std::unique_ptr<TransformationMatrix>,
               const base::Optional<gpu::MailboxHolder>& output_mailbox_holder,
               const base::Optional<gpu::MailboxHolder>& bg_mailbox_holder,
               const base::Optional<IntSize>& background_size);
  void OnInputStateChange(
      int16_t frame_id,
      const WTF::Vector<device::mojom::blink::XRInputSourceStatePtr>&);

  const HeapVector<Member<XRView>>& views();

  void OnSelectStart(XRInputSource*);
  void OnSelectEnd(XRInputSource*);
  void OnSelect(XRInputSource*);

  void OnPoseReset();

  const device::mojom::blink::VRDisplayInfoPtr& GetVRDisplayInfo() {
    return display_info_;
  }
  bool External() const { return is_external_; }
  // Incremented every time display_info_ is changed, so that other objects that
  // depend on it can know when they need to update.
  unsigned int DisplayInfoPtrId() const { return display_info_id_; }

  void SetNonImmersiveProjectionMatrix(const WTF::Vector<float>&);
  void SetXRDisplayInfo(device::mojom::blink::VRDisplayInfoPtr display_info);

  void Trace(blink::Visitor*) override;

  // ScriptWrappable
  bool HasPendingActivity() const override;

 private:
  class XRSessionResizeObserverDelegate;

  XRFrame* CreatePresentationFrame();
  void UpdateCanvasDimensions(Element*);

  void UpdateInputSourceState(
      XRInputSource*,
      const device::mojom::blink::XRInputSourceStatePtr&);
  XRInputSourceEvent* CreateInputSourceEvent(const AtomicString&,
                                             XRInputSource*);

  // XRSessionClient
  void OnChanged(device::mojom::blink::VRDisplayInfoPtr) override;
  void OnExitPresent() override;
  void OnFocus() override;
  void OnBlur() override;

  bool HasAppropriateFocus();

  void OnHitTestResults(
      ScriptPromiseResolver* resolver,
      base::Optional<WTF::Vector<device::mojom::blink::XRHitResultPtr>>
          results);

  const Member<XRDevice> device_;
  const bool immersive_;
  const bool environment_integration_;
  const Member<XRPresentationContext> output_context_;
  String blend_mode_string_;
  Member<XRLayer> base_layer_;
  HeapVector<Member<XRView>> views_;
  InputSourceMap input_sources_;
  Member<ResizeObserver> resize_observer_;
  Member<XRCanvasInputProvider> canvas_input_provider_;

  bool has_device_focus_ = true;
  bool is_external_ = false;
  int display_info_id_ = 0;
  device::mojom::blink::VRDisplayInfoPtr display_info_;

  mojo::Binding<device::mojom::blink::XRSessionClient> client_binding_;

  TraceWrapperMember<XRFrameRequestCallbackCollection> callback_collection_;
  std::unique_ptr<TransformationMatrix> base_pose_matrix_;

  WTF::Vector<float> non_immersive_projection_matrix_;

  double depth_near_ = 0.1;
  double depth_far_ = 1000.0;
  bool blurred_;
  bool ended_ = false;
  bool pending_frame_ = false;
  bool resolving_frame_ = false;
  bool update_views_next_frame_ = false;
  bool views_dirty_ = true;

  // Indicates that we've already logged a metric, so don't need to log it
  // again.
  mutable bool did_log_getInputSources_ = false;
  mutable bool did_log_getDevicePose_ = false;

  // Dimensions of the output canvas.
  int output_width_ = 1;
  int output_height_ = 1;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_XR_XR_SESSION_H_
