// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_NAVIGATION_PARAMS_H_
#define THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_NAVIGATION_PARAMS_H_

#include <memory>

#include "base/optional.h"
#include "base/time/time.h"
#include "mojo/public/cpp/system/message_pipe.h"
#include "third_party/blink/public/platform/modules/service_worker/web_service_worker_network_provider.h"
#include "third_party/blink/public/platform/web_common.h"
#include "third_party/blink/public/platform/web_content_security_policy.h"
#include "third_party/blink/public/platform/web_source_location.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/platform/web_url_request.h"
#include "third_party/blink/public/web/web_form_element.h"
#include "third_party/blink/public/web/web_frame_load_type.h"
#include "third_party/blink/public/web/web_navigation_policy.h"
#include "third_party/blink/public/web/web_navigation_timings.h"
#include "third_party/blink/public/web/web_navigation_type.h"
#include "third_party/blink/public/web/web_triggering_event_info.h"

namespace blink {

// This structure holds all information collected by Blink when
// navigation is being initiated.
struct BLINK_EXPORT WebNavigationInfo {
  WebNavigationInfo() = default;
  ~WebNavigationInfo() = default;

  // The main resource request.
  WebURLRequest url_request;

  // The navigation type. See WebNavigationType.
  WebNavigationType navigation_type = kWebNavigationTypeOther;

  // The navigation policy. See WebNavigationPolicy.
  WebNavigationPolicy navigation_policy = kWebNavigationPolicyCurrentTab;

  // Whether the frame had a transient user activation
  // at the time this request was issued.
  bool has_transient_user_activation = false;

  // The load type. See WebFrameLoadType.
  WebFrameLoadType frame_load_type = WebFrameLoadType::kStandard;

  // During a history load, a child frame can be initially navigated
  // to an url from the history state. This flag indicates it.
  bool is_history_navigation_in_new_child_frame = false;

  // Whether the navigation is a result of client redirect.
  bool is_client_redirect = false;

  // Whether this is a navigation in the opener frame initiated
  // by the window.open'd frame.
  bool is_opener_navigation = false;

  // Event information. See WebTriggeringEventInfo.
  WebTriggeringEventInfo triggering_event_info =
      WebTriggeringEventInfo::kUnknown;

  // If the navigation is a result of form submit, the form element is provided.
  WebFormElement form;

  // The location in the source which triggered the navigation.
  // Used to help web developers understand what caused the navigation.
  WebSourceLocation source_location;

  // The initiator of this navigation used by DevTools.
  WebString devtools_initiator_info;

  // Whether this navigation should check CSP. See
  // WebContentSecurityPolicyDisposition.
  WebContentSecurityPolicyDisposition
      should_check_main_world_content_security_policy =
          kWebContentSecurityPolicyDispositionCheck;

  // When navigating to a blob url, this token specifies the blob.
  mojo::ScopedMessagePipeHandle blob_url_token;

  // When navigation initiated from the user input, this tracks
  // the input start time.
  base::TimeTicks input_start;

  // The navigation initiator, if any.
  mojo::ScopedMessagePipeHandle navigation_initiator_handle;

  // Specifies whether or not a MHTML Archive can be used to load a subframe
  // resource instead of doing a network request.
  enum class ArchiveStatus { Absent, Present };
  ArchiveStatus archive_status = ArchiveStatus::Absent;

  // The value of hrefTranslate attribute of a link, if this navigation was
  // inititated by clicking a link.
  WebString href_translate;
};

// This structure holds all information provided by the embedder that is
// needed for blink to load a Document. This is hence different from
// WebDocumentLoader::ExtraData, which is an opaque structure stored in the
// DocumentLoader and used by the embedder.
struct BLINK_EXPORT WebNavigationParams {
  WebNavigationTimings navigation_timings;
  base::Optional<WebSourceLocation> source_location;
  bool is_user_activated = false;
  std::unique_ptr<blink::WebServiceWorkerNetworkProvider>
      service_worker_network_provider;
};

}  // namespace blink

#endif
