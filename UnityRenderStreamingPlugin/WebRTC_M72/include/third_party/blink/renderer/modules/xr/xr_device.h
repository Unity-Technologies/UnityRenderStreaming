// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_XR_XR_DEVICE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_XR_XR_DEVICE_H_

#include "device/vr/public/mojom/vr_service.mojom-blink.h"
#include "mojo/public/cpp/bindings/binding.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise_resolver.h"
#include "third_party/blink/renderer/modules/xr/xr_session_creation_options.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class ScriptPromiseResolver;
class XR;
class XRFrameProvider;
class XRSession;

class XRDevice final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  XRDevice(XR*, device::mojom::blink::XRDevicePtr);
  XR* xr() const { return xr_; }

  ScriptPromise supportsSession(ScriptState*, const XRSessionCreationOptions*);
  ScriptPromise requestSession(ScriptState*, const XRSessionCreationOptions*);

  XRFrameProvider* frameProvider();

  void Dispose();

  const device::mojom::blink::XRDevicePtr& xrDevicePtr() const {
    return device_ptr_;
  }
  const device::mojom::blink::XRFrameDataProviderPtr& xrMagicWindowProviderPtr()
      const {
    return magic_window_provider_;
  }
  const device::mojom::blink::XREnvironmentIntegrationProviderAssociatedPtr&
  xrEnvironmentProviderPtr();

  void OnFrameFocusChanged();
  bool HasFrameFocus() { return IsFrameFocused(); }

  int64_t GetSourceId() const;

  void Trace(blink::Visitor*) override;

 private:
  const char* checkSessionSupport(const XRSessionCreationOptions*) const;

  void OnRequestSessionReturned(ScriptPromiseResolver* resolver,
                                XRPresentationContext* output_context,
                                bool environment_integration,
                                bool immersive,
                                device::mojom::blink::XRSessionPtr session);
  void OnSupportsSessionReturned(ScriptPromiseResolver* resolver,
                                 bool supports_session);

  // There are two components to focus - whether the frame itself has
  // traditional focus and whether the device reports that we have focus. These
  // are aggregated so we can hand out focus/blur events on sessions and
  // determine when to call animation frame callbacks.
  void OnFocusChanged();
  bool IsFrameFocused();

  Member<XR> xr_;
  Member<XRFrameProvider> frame_provider_;
  HeapHashSet<WeakMember<XRSession>> sessions_;

  // Indicates whether we've already logged a request for an immersive session.
  bool did_log_request_immersive_session_ = false;

  device::mojom::blink::XRFrameDataProviderPtr magic_window_provider_;
  device::mojom::blink::XREnvironmentIntegrationProviderAssociatedPtr
      environment_provider_;
  device::mojom::blink::XRDevicePtr device_ptr_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_XR_XR_DEVICE_H_
