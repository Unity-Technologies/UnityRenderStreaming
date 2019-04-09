/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_SECURITY_POLICY_H_
#define THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_SECURITY_POLICY_H_

#include "services/network/public/mojom/cors_origin_pattern.mojom-shared.h"
#include "services/network/public/mojom/referrer_policy.mojom-shared.h"
#include "third_party/blink/public/platform/web_common.h"

namespace blink {

class WebString;
class WebURL;

class WebSecurityPolicy {
 public:
  // Registers a URL scheme to be treated as display-isolated. This means
  // that pages cannot display these URLs unless they are from the same
  // scheme. For example, pages in other origin cannot create iframes or
  // hyperlinks to URLs with the scheme.
  BLINK_EXPORT static void RegisterURLSchemeAsDisplayIsolated(const WebString&);

  // Registers a URL scheme that can register a ServiceWorker.
  BLINK_EXPORT static void RegisterURLSchemeAsAllowingServiceWorkers(
      const WebString&);

  // Registers an URL scheme as allowing 'wasm-eval' CSP source directive.
  BLINK_EXPORT static void RegisterURLSchemeAsAllowingWasmEvalCSP(
      const WebString&);

  // Registers an HTTP-like URL scheme that supports the Fetch API.
  BLINK_EXPORT static void RegisterURLSchemeAsSupportingFetchAPI(
      const WebString&);

  // Registers a URL scheme which will always be considered the first-party when
  // loaded in a top-level context.
  BLINK_EXPORT static void RegisterURLSchemeAsFirstPartyWhenTopLevel(
      const WebString&);

  // Support for managing allow/block access lists to origins beyond the
  // same-origin policy. The block list takes priority over the allow list.
  // When an origin matches an entry on both the allow list and block list
  // the entry with the higher priority defines whether access is allowed.
  // In the case where both an allowlist and blocklist rule of the same
  // priority match a request the blocklist rule takes priority.
  // Callers should use
  // network::mojom::CorsOriginAccessMatchPriority::kDefaultPriority as the
  // default priority unless overriding existing entries is explicitly needed.
  BLINK_EXPORT static void AddOriginAccessAllowListEntry(
      const WebURL& source_origin,
      const WebString& destination_protocol,
      const WebString& destination_host,
      bool allow_destination_subdomains,
      const network::mojom::CorsOriginAccessMatchPriority priority);
  BLINK_EXPORT static void AddOriginAccessBlockListEntry(
      const WebURL& source_origin,
      const WebString& destination_protocol,
      const WebString& destination_host,
      bool disallow_destination_subdomains,
      const network::mojom::CorsOriginAccessMatchPriority priority);
  BLINK_EXPORT static void ClearOriginAccessListForOrigin(
      const WebURL& source_origin);
  BLINK_EXPORT static void ClearOriginAccessList();

  // Support for whitelisting origins or hostname patterns to treat them as
  // trustworthy. This method does not do any canonicalization; the caller is
  // responsible for canonicalizing them before calling this.
  BLINK_EXPORT static void AddOriginTrustworthyWhiteList(const WebString&);

  // Support for whitelisting schemes as bypassing secure context checks.
  BLINK_EXPORT static void AddSchemeToBypassSecureContextWhitelist(
      const WebString&);

  // Returns the referrer modified according to the referrer policy for a
  // navigation to a given URL. If the referrer returned is empty, the
  // referrer header should be omitted.
  BLINK_EXPORT static WebString GenerateReferrerHeader(
      network::mojom::ReferrerPolicy,
      const WebURL&,
      const WebString& referrer);

  // Registers an URL scheme to not allow manipulation of the loaded page
  // by bookmarklets or javascript: URLs typed in the omnibox.
  BLINK_EXPORT static void RegisterURLSchemeAsNotAllowingJavascriptURLs(
      const WebString&);

  // Registers an URL scheme as allowed in referrers.
  BLINK_EXPORT static void RegisterURLSchemeAsAllowedForReferrer(
      const WebString&);

 private:
  WebSecurityPolicy() = delete;
};

}  // namespace blink

#endif
