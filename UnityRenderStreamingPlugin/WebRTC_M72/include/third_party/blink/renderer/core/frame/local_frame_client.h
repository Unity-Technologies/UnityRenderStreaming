/*
 * Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011, 2012 Apple Inc. All rights
 * reserved.
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
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_LOCAL_FRAME_CLIENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_LOCAL_FRAME_CLIENT_H_

#include <memory>

#include "third_party/blink/public/common/feature_policy/feature_policy.h"
#include "third_party/blink/public/mojom/frame/navigation_initiator.mojom-blink.h"
#include "third_party/blink/public/platform/web_content_security_policy_struct.h"
#include "third_party/blink/public/platform/web_content_settings_client.h"
#include "third_party/blink/public/platform/web_effective_connection_type.h"
#include "third_party/blink/public/platform/web_insecure_request_policy.h"
#include "third_party/blink/public/platform/web_loading_behavior_flag.h"
#include "third_party/blink/public/platform/web_scoped_virtual_time_pauser.h"
#include "third_party/blink/public/platform/web_sudden_termination_disabler_type.h"
#include "third_party/blink/public/platform/web_url_request.h"
#include "third_party/blink/public/platform/web_worker_fetch_context.h"
#include "third_party/blink/public/web/web_frame_load_type.h"
#include "third_party/blink/public/web/web_global_object_reuse_policy.h"
#include "third_party/blink/public/web/web_history_commit_type.h"
#include "third_party/blink/public/web/web_navigation_params.h"
#include "third_party/blink/public/web/web_triggering_event_info.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/icon_url.h"
#include "third_party/blink/renderer/core/frame/frame_client.h"
#include "third_party/blink/renderer/core/frame/frame_types.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/html/link_resource.h"
#include "third_party/blink/renderer/core/loader/frame_load_request.h"
#include "third_party/blink/renderer/core/loader/frame_loader_types.h"
#include "third_party/blink/renderer/core/loader/navigation_policy.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource_load_priority.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource_loader_options.h"
#include "third_party/blink/renderer/platform/network/content_security_policy_parsers.h"
#include "third_party/blink/renderer/platform/weborigin/referrer.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"
#include "v8/include/v8.h"

namespace service_manager {
class InterfaceProvider;
}  // namespace service_manager

namespace blink {
namespace mojom {
enum class WebFeature : int32_t;
}  // namespace mojom

class AssociatedInterfaceProvider;
class Document;
class DocumentLoader;
class HTMLFormElement;
class HTMLFrameOwnerElement;
class HTMLMediaElement;
class HTMLPlugInElement;
class HistoryItem;
class KURL;
class WebPluginContainerImpl;
class ResourceError;
class ResourceRequest;
class ResourceResponse;
class SecurityOrigin;
class SharedWorkerRepositoryClient;
class SubstituteData;
class WebApplicationCacheHost;
class WebApplicationCacheHostClient;
class WebCookieJar;
class WebLayerTreeView;
class WebLocalFrame;
class WebMediaPlayer;
class WebMediaPlayerClient;
class WebMediaPlayerSource;
class WebRTCPeerConnectionHandler;
class WebRemotePlaybackClient;
struct WebResourceTimingInfo;
class WebServiceWorkerProvider;
class WebSpellCheckPanelHostClient;
struct WebScrollIntoViewParams;
class WebTextCheckClient;

// Whether to follow cross origin redirects when downloading, or treating
// the download as a navigation instead.
enum class DownloadCrossOriginRedirects { kFollow, kNavigate };

class CORE_EXPORT LocalFrameClient : public FrameClient {
 public:
  ~LocalFrameClient() override = default;

  virtual WebLocalFrame* GetWebFrame() const { return nullptr; }

  virtual bool HasWebView() const = 0;  // mainly for assertions

  virtual void WillBeDetached() = 0;
  virtual void DispatchWillSendRequest(ResourceRequest&) = 0;
  virtual void DispatchDidReceiveResponse(const ResourceResponse&) = 0;
  virtual void DispatchDidLoadResourceFromMemoryCache(
      const ResourceRequest&,
      const ResourceResponse&) = 0;

  virtual void DispatchDidHandleOnloadEvents() = 0;
  virtual void DidFinishSameDocumentNavigation(HistoryItem*,
                                               WebHistoryCommitType,
                                               bool content_initiated) {}
  virtual void DispatchWillCommitProvisionalLoad() = 0;
  virtual void DispatchDidStartProvisionalLoad(DocumentLoader*,
                                               const ResourceRequest&) = 0;
  virtual void DispatchDidReceiveTitle(const String&) = 0;
  virtual void DispatchDidChangeIcons(IconType) = 0;
  virtual void DispatchDidCommitLoad(HistoryItem*,
                                     WebHistoryCommitType,
                                     WebGlobalObjectReusePolicy) = 0;
  virtual void DispatchDidFailProvisionalLoad(const ResourceError&,
                                              WebHistoryCommitType) = 0;
  virtual void DispatchDidFailLoad(const ResourceError&,
                                   WebHistoryCommitType) = 0;
  virtual void DispatchDidFinishDocumentLoad() = 0;
  virtual void DispatchDidFinishLoad() = 0;
  virtual void DispatchDidChangeThemeColor() = 0;

  virtual void BeginNavigation(
      const ResourceRequest&,
      Document* origin_document,
      DocumentLoader*,
      WebNavigationType,
      NavigationPolicy,
      bool has_transient_activation,
      WebFrameLoadType,
      bool is_client_redirect,
      WebTriggeringEventInfo,
      HTMLFormElement*,
      ContentSecurityPolicyDisposition
          should_check_main_world_content_security_policy,
      mojom::blink::BlobURLTokenPtr,
      base::TimeTicks input_start_time,
      const String& href_translate,
      mojom::blink::NavigationInitiatorPtr) = 0;

  virtual void DispatchWillSendSubmitEvent(HTMLFormElement*) = 0;

  virtual void DidStartLoading() = 0;
  virtual void ProgressEstimateChanged(double progress_estimate) = 0;
  virtual void DidStopLoading() = 0;

  virtual void ForwardResourceTimingToParent(const WebResourceTimingInfo&) = 0;

  virtual void DownloadURL(const ResourceRequest&,
                           DownloadCrossOriginRedirects) = 0;
  virtual void LoadErrorPage(int reason) = 0;

  virtual bool NavigateBackForward(int offset) const = 0;

  // Another page has accessed the initial empty document of this frame. It is
  // no longer safe to display a provisional URL, since a URL spoof is now
  // possible.
  virtual void DidAccessInitialDocument() {}

  // This frame has displayed inactive content (such as an image) from an
  // insecure source.  Inactive content cannot spread to other frames.
  virtual void DidDisplayInsecureContent() = 0;

  // This frame contains a form that submits to an insecure target url.
  virtual void DidContainInsecureFormAction() = 0;

  // The indicated security origin has run active content (such as a script)
  // from an insecure source.  Note that the insecure content can spread to
  // other frames in the same origin.
  virtual void DidRunInsecureContent(const SecurityOrigin*, const KURL&) = 0;
  virtual void DidDispatchPingLoader(const KURL&) = 0;

  // The frame displayed content with certificate errors with given URL.
  virtual void DidDisplayContentWithCertificateErrors() = 0;
  // The frame ran content with certificate errors with the given URL.
  virtual void DidRunContentWithCertificateErrors() = 0;

  // The frame loaded a resource with a legacy Symantec certificate that is
  // slated for distrust (indicated by |did_fail| being false) or has already
  // been distrusted (indicated by |did_fail| being true). Prints a console
  // message (possibly overridden by the embedder) to warn about the
  // certificate.
  virtual void ReportLegacySymantecCert(const KURL&, bool did_fail) {}

  // The frame loaded a resource with a legacy TLS version that will be removed
  // in the future. Prints a console message to warn about this.
  virtual void ReportLegacyTLSVersion(const KURL&) {}

  // Will be called when |PerformanceTiming| events are updated
  virtual void DidChangePerformanceTiming() {}

  // Will be called when a particular loading code path has been used. This
  // propogates renderer loading behavior to the browser process for histograms.
  virtual void DidObserveLoadingBehavior(WebLoadingBehaviorFlag) {}

  // Will be called when a new UseCounter feature has been observed in a frame.
  // This propogates feature usage to the browser process for histograms.
  virtual void DidObserveNewFeatureUsage(mojom::WebFeature) {}
  // Will be called when a new UseCounter CSS property or animated CSS property
  // has been observed in a frame. This propogates feature usage to the browser
  // process for histograms.
  virtual void DidObserveNewCssPropertyUsage(int /*css_property*/,
                                             bool /*is_animated*/) {}

  // Reports that visible elements in the frame shifted (bit.ly/lsm-explainer).
  virtual void DidObserveLayoutJank(double jank_fraction) {}

  // Will be called by a Page upon DidCommitLoad, deciding whether to track
  // UseCounter usage or not based on its url.
  virtual bool ShouldTrackUseCounter(const KURL&) { return true; }

  // Transmits the change in the set of watched CSS selectors property that
  // match any element on the frame.
  virtual void SelectorMatchChanged(
      const Vector<String>& added_selectors,
      const Vector<String>& removed_selectors) = 0;

  virtual DocumentLoader* CreateDocumentLoader(
      LocalFrame*,
      const ResourceRequest&,
      const SubstituteData&,
      ClientRedirectPolicy,
      const base::UnguessableToken& devtools_navigation_token,
      WebFrameLoadType,
      WebNavigationType,
      std::unique_ptr<WebNavigationParams> navigation_params,
      std::unique_ptr<WebDocumentLoader::ExtraData> extra_data) = 0;

  virtual void UpdateDocumentLoader(
      DocumentLoader* document_loader,
      std::unique_ptr<WebDocumentLoader::ExtraData> extra_data) = 0;

  virtual String UserAgent() = 0;

  virtual String DoNotTrackValue() = 0;

  virtual void TransitionToCommittedForNewPage() = 0;

  virtual LocalFrame* CreateFrame(const AtomicString& name,
                                  HTMLFrameOwnerElement*) = 0;

  // Whether or not plugin creation should fail if the HTMLPlugInElement isn't
  // in the DOM after plugin initialization.
  enum DetachedPluginPolicy {
    kFailOnDetachedPlugin,
    kAllowDetachedPlugin,
  };
  virtual WebPluginContainerImpl* CreatePlugin(HTMLPlugInElement&,
                                               const KURL&,
                                               const Vector<String>&,
                                               const Vector<String>&,
                                               const String&,
                                               bool load_manually) = 0;

  virtual std::unique_ptr<WebMediaPlayer> CreateWebMediaPlayer(
      HTMLMediaElement&,
      const WebMediaPlayerSource&,
      WebMediaPlayerClient*,
      WebLayerTreeView*) = 0;
  virtual WebRemotePlaybackClient* CreateWebRemotePlaybackClient(
      HTMLMediaElement&) = 0;

  virtual void DidCreateNewDocument() = 0;
  virtual void DispatchDidClearWindowObjectInMainWorld() = 0;
  virtual void DocumentElementAvailable() = 0;
  virtual void RunScriptsAtDocumentElementAvailable() = 0;
  virtual void RunScriptsAtDocumentReady(bool document_is_empty) = 0;
  virtual void RunScriptsAtDocumentIdle() = 0;

  virtual void DidCreateScriptContext(v8::Local<v8::Context>, int world_id) = 0;
  virtual void WillReleaseScriptContext(v8::Local<v8::Context>,
                                        int world_id) = 0;
  virtual bool AllowScriptExtensions() = 0;

  virtual void DidChangeScrollOffset() {}
  virtual void DidUpdateCurrentHistoryItem() {}

  // Called when a content-initiated, main frame navigation to a data URL is
  // about to occur.
  virtual bool AllowContentInitiatedDataUrlNavigations(const KURL&) {
    return false;
  }

  virtual WebCookieJar* CookieJar() const = 0;

  virtual void DidChangeName(const String&) {}

  virtual void DidEnforceInsecureRequestPolicy(WebInsecureRequestPolicy) {}
  virtual void DidEnforceInsecureNavigationsSet(const std::vector<unsigned>&) {}

  virtual void DidChangeFramePolicy(Frame* child_frame,
                                    SandboxFlags,
                                    const ParsedFeaturePolicy&) {}

  virtual void DidSetFramePolicyHeaders(
      SandboxFlags,
      const ParsedFeaturePolicy& parsed_header) {}

  // Called when a set of new Content Security Policies is added to the frame's
  // document. This can be triggered by handling of HTTP headers, handling of
  // <meta> element, or by inheriting CSP from the parent (in case of
  // about:blank).
  virtual void DidAddContentSecurityPolicies(
      const blink::WebVector<WebContentSecurityPolicy>&) {}

  virtual void DidChangeFrameOwnerProperties(HTMLFrameOwnerElement*) {}

  virtual void DispatchWillStartUsingPeerConnectionHandler(
      WebRTCPeerConnectionHandler*) {}

  virtual bool ShouldBlockWebGL() { return false; }

  virtual std::unique_ptr<WebServiceWorkerProvider>
  CreateServiceWorkerProvider() = 0;

  virtual WebContentSettingsClient* GetContentSettingsClient() = 0;

  virtual SharedWorkerRepositoryClient* GetSharedWorkerRepositoryClient() {
    return nullptr;
  }

  virtual std::unique_ptr<WebApplicationCacheHost> CreateApplicationCacheHost(
      WebApplicationCacheHostClient*) = 0;

  virtual void DispatchDidChangeManifest() {}

  unsigned BackForwardLength() override { return 0; }

  virtual bool IsLocalFrameClientImpl() const { return false; }

  virtual void SuddenTerminationDisablerChanged(
      bool present,
      WebSuddenTerminationDisablerType) {}

  // Effective connection type when this frame was loaded.
  virtual WebEffectiveConnectionType GetEffectiveConnectionType() {
    return WebEffectiveConnectionType::kTypeUnknown;
  }
  // Overrides the effective connection type for testing.
  virtual void SetEffectiveConnectionTypeForTesting(
      WebEffectiveConnectionType) {}

  // Returns the PreviewsState active for the frame.
  virtual WebURLRequest::PreviewsState GetPreviewsStateForFrame() const {
    return WebURLRequest::kPreviewsUnspecified;
  }

  // Overwrites the given URL to use an HTML5 embed if possible. An empty URL is
  // returned if the URL is not overriden.
  virtual KURL OverrideFlashEmbedWithHTML(const KURL&) { return KURL(); }

  virtual BlameContext* GetFrameBlameContext() { return nullptr; }

  virtual service_manager::InterfaceProvider* GetInterfaceProvider() {
    return nullptr;
  }

  virtual AssociatedInterfaceProvider*
  GetRemoteNavigationAssociatedInterfaces() {
    return nullptr;
  }

  // Notify the embedder that the associated frame has user activation so that
  // the replicated states in the browser and other renderers can be updated.
  virtual void NotifyUserActivation() {}

  // Tell the embedder that the associated frame has consumed user activation so
  // that the replicated states in the browser and other renderers can be
  // updated.
  virtual void ConsumeUserActivation() {}

  virtual void SetHasReceivedUserGestureBeforeNavigation(bool value) {}

  virtual void AbortClientNavigation() {}

  virtual WebSpellCheckPanelHostClient* SpellCheckPanelHostClient() const = 0;

  virtual WebTextCheckClient* GetTextCheckerClient() const = 0;

  virtual std::unique_ptr<WebURLLoaderFactory> CreateURLLoaderFactory() = 0;

  virtual void AnnotatedRegionsChanged() = 0;

  virtual void DidBlockFramebust(const KURL&) {}

  // Called when the corresponding frame should be scrolled in a remote parent
  // frame.
  virtual void ScrollRectToVisibleInParentFrame(
      const WebRect&,
      const WebScrollIntoViewParams&) {}

  virtual void BubbleLogicalScrollInParentFrame(
      ScrollDirection direction,
      ScrollGranularity granularity) = 0;

  virtual void SetVirtualTimePauser(
      WebScopedVirtualTimePauser virtual_time_pauser) {}

  virtual String evaluateInInspectorOverlayForTesting(const String& script) = 0;

  virtual bool HandleCurrentKeyboardEvent() { return false; }

  virtual void DidChangeSelection(bool is_selection_empty) {}

  virtual void DidChangeContents() {}

  virtual Frame* FindFrame(const AtomicString& name) const = 0;

  virtual void FrameRectsChanged(const IntRect&) {}

  // Returns true when the contents of plugin are handled externally. This means
  // the plugin element will own a content frame but the frame is than used
  // externally to load the required handelrs.
  virtual bool IsPluginHandledExternally(HTMLPlugInElement&,
                                         const KURL&,
                                         const String&) {
    return false;
  };

  // Returns a new WebWorkerFetchContext for a dedicated worker or worklet.
  virtual scoped_refptr<WebWorkerFetchContext> CreateWorkerFetchContext() {
    return nullptr;
  }

  virtual std::unique_ptr<WebContentSettingsClient>
  CreateWorkerContentSettingsClient() {
    return nullptr;
  }

  virtual void SetMouseCapture(bool) {}

  // Returns whether we are associated with a print context who suggests to use
  // printing layout.
  virtual bool UsePrintingLayout() const { return false; }
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_LOCAL_FRAME_CLIENT_H_
