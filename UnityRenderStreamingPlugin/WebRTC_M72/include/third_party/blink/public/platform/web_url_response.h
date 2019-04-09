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

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_URL_RESPONSE_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_URL_RESPONSE_H_

#include <memory>

#include "base/time/time.h"
#include "net/cert/ct_policy_status.h"
#include "net/http/http_response_info.h"
#include "services/network/public/mojom/fetch_api.mojom-shared.h"
#include "third_party/blink/public/platform/modules/fetch/fetch_api_request.mojom-shared.h"
#include "third_party/blink/public/platform/web_common.h"
#include "third_party/blink/public/platform/web_security_style.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/platform/web_vector.h"

namespace blink {

class ResourceResponse;
class WebHTTPHeaderVisitor;
class WebHTTPLoadInfo;
class WebURL;
class WebURLLoadTiming;

class WebURLResponse {
 public:
  enum HTTPVersion {
    kHTTPVersionUnknown,
    kHTTPVersion_0_9,
    kHTTPVersion_1_0,
    kHTTPVersion_1_1,
    kHTTPVersion_2_0
  };

  struct SignedCertificateTimestamp {
    SignedCertificateTimestamp() = default;
    SignedCertificateTimestamp(WebString status,
                               WebString origin,
                               WebString log_description,
                               WebString log_id,
                               int64_t timestamp,
                               WebString hash_algorithm,
                               WebString signature_algorithm,
                               WebString signature_data)
        : status(status),
          origin(origin),
          log_description(log_description),
          log_id(log_id),
          timestamp(timestamp),
          hash_algorithm(hash_algorithm),
          signature_algorithm(signature_algorithm),
          signature_data(signature_data) {}
    WebString status;
    WebString origin;
    WebString log_description;
    WebString log_id;
    int64_t timestamp;
    WebString hash_algorithm;
    WebString signature_algorithm;
    WebString signature_data;
  };

  using SignedCertificateTimestampList = WebVector<SignedCertificateTimestamp>;

  struct WebSecurityDetails {
    WebSecurityDetails(const WebString& protocol,
                       const WebString& key_exchange,
                       const WebString& key_exchange_group,
                       const WebString& cipher,
                       const WebString& mac,
                       const WebString& subject_name,
                       const WebVector<WebString>& san_list,
                       const WebString& issuer,
                       double valid_from,
                       double valid_to,
                       const WebVector<WebString>& certificate,
                       const SignedCertificateTimestampList& sct_list)
        : protocol(protocol),
          key_exchange(key_exchange),
          key_exchange_group(key_exchange_group),
          cipher(cipher),
          mac(mac),
          subject_name(subject_name),
          san_list(san_list),
          issuer(issuer),
          valid_from(valid_from),
          valid_to(valid_to),
          certificate(certificate),
          sct_list(sct_list) {}
    // All strings are human-readable values.
    WebString protocol;
    // keyExchange is the empty string if not applicable for the connection's
    // protocol.
    WebString key_exchange;
    // keyExchangeGroup is the empty string if not applicable for the
    // connection's key exchange.
    WebString key_exchange_group;
    WebString cipher;
    // mac is the empty string when the connection cipher suite does not
    // have a separate MAC value (i.e. if the cipher suite is AEAD).
    WebString mac;
    WebString subject_name;
    WebVector<WebString> san_list;
    WebString issuer;
    double valid_from;
    double valid_to;
    // DER-encoded X509Certificate certificate chain.
    WebVector<WebString> certificate;
    SignedCertificateTimestampList sct_list;
  };

  class ExtraData {
   public:
    virtual ~ExtraData() = default;
  };

  BLINK_PLATFORM_EXPORT ~WebURLResponse();

  BLINK_PLATFORM_EXPORT WebURLResponse();
  BLINK_PLATFORM_EXPORT WebURLResponse(const WebURLResponse&);
  BLINK_PLATFORM_EXPORT explicit WebURLResponse(const WebURL&);
  BLINK_PLATFORM_EXPORT WebURLResponse& operator=(const WebURLResponse&);

  BLINK_PLATFORM_EXPORT bool IsNull() const;

  BLINK_PLATFORM_EXPORT WebURL Url() const;
  BLINK_PLATFORM_EXPORT void SetURL(const WebURL&);

  BLINK_PLATFORM_EXPORT void SetConnectionID(unsigned);

  BLINK_PLATFORM_EXPORT void SetConnectionReused(bool);

  BLINK_PLATFORM_EXPORT void SetLoadTiming(const WebURLLoadTiming&);

  BLINK_PLATFORM_EXPORT void SetHTTPLoadInfo(const WebHTTPLoadInfo&);

  BLINK_PLATFORM_EXPORT void SetResponseTime(base::Time);

  BLINK_PLATFORM_EXPORT WebString MimeType() const;
  BLINK_PLATFORM_EXPORT void SetMIMEType(const WebString&);

  BLINK_PLATFORM_EXPORT long long ExpectedContentLength() const;
  BLINK_PLATFORM_EXPORT void SetExpectedContentLength(long long);

  BLINK_PLATFORM_EXPORT void SetTextEncodingName(const WebString&);

  BLINK_PLATFORM_EXPORT HTTPVersion HttpVersion() const;
  BLINK_PLATFORM_EXPORT void SetHTTPVersion(HTTPVersion);

  BLINK_PLATFORM_EXPORT int RequestId() const;
  BLINK_PLATFORM_EXPORT void SetRequestId(int);

  BLINK_PLATFORM_EXPORT int HttpStatusCode() const;
  BLINK_PLATFORM_EXPORT void SetHTTPStatusCode(int);

  BLINK_PLATFORM_EXPORT WebString HttpStatusText() const;
  BLINK_PLATFORM_EXPORT void SetHTTPStatusText(const WebString&);

  BLINK_PLATFORM_EXPORT WebString HttpHeaderField(const WebString& name) const;
  BLINK_PLATFORM_EXPORT void SetHTTPHeaderField(const WebString& name,
                                                const WebString& value);
  BLINK_PLATFORM_EXPORT void AddHTTPHeaderField(const WebString& name,
                                                const WebString& value);
  BLINK_PLATFORM_EXPORT void ClearHTTPHeaderField(const WebString& name);
  BLINK_PLATFORM_EXPORT void VisitHTTPHeaderFields(WebHTTPHeaderVisitor*) const;

  BLINK_PLATFORM_EXPORT long long AppCacheID() const;
  BLINK_PLATFORM_EXPORT void SetAppCacheID(long long);

  BLINK_PLATFORM_EXPORT WebURL AppCacheManifestURL() const;
  BLINK_PLATFORM_EXPORT void SetAppCacheManifestURL(const WebURL&);

  BLINK_PLATFORM_EXPORT void SetHasMajorCertificateErrors(bool);
  BLINK_PLATFORM_EXPORT void SetCTPolicyCompliance(net::ct::CTPolicyCompliance);
  BLINK_PLATFORM_EXPORT void SetIsLegacySymantecCert(bool);
  BLINK_PLATFORM_EXPORT void SetIsLegacyTLSVersion(bool);

  BLINK_PLATFORM_EXPORT void SetSecurityStyle(WebSecurityStyle);

  BLINK_PLATFORM_EXPORT void SetSecurityDetails(const WebSecurityDetails&);
  BLINK_PLATFORM_EXPORT WebSecurityDetails SecurityDetailsForTesting();

  BLINK_PLATFORM_EXPORT void SetAsyncRevalidationRequested(bool);
  BLINK_PLATFORM_EXPORT void SetNetworkAccessed(bool);

#if INSIDE_BLINK
  BLINK_PLATFORM_EXPORT const ResourceResponse& ToResourceResponse() const;
#endif

  // Flag whether this request was served from the disk cache entry.
  BLINK_PLATFORM_EXPORT void SetWasCached(bool);

  // Flag whether this request was loaded via the SPDY protocol or not.
  // SPDY is an experimental web protocol, see http://dev.chromium.org/spdy
  BLINK_PLATFORM_EXPORT void SetWasFetchedViaSPDY(bool);

  // Flag whether this request was loaded via a ServiceWorker. See
  // ServiceWorkerResponseInfo::was_fetched_via_service_worker() for details.
  BLINK_PLATFORM_EXPORT bool WasFetchedViaServiceWorker() const;
  BLINK_PLATFORM_EXPORT void SetWasFetchedViaServiceWorker(bool);

  // Flag whether the fallback request with skip service worker flag was
  // required. See ServiceWorkerResponseInfo::was_fallback_required() for
  // details.
  BLINK_PLATFORM_EXPORT void SetWasFallbackRequiredByServiceWorker(bool);

  // https://fetch.spec.whatwg.org/#concept-response-type
  BLINK_PLATFORM_EXPORT void SetType(network::mojom::FetchResponseType);
  BLINK_PLATFORM_EXPORT network::mojom::FetchResponseType GetType() const;

  // The URL list of the Response object the ServiceWorker passed to
  // respondWith(). See ServiceWorkerResponseInfo::url_list_via_service_worker()
  // for details.
  BLINK_PLATFORM_EXPORT void SetURLListViaServiceWorker(
      const WebVector<WebURL>&);

  // Returns the last URL of the URL list of the Response object the
  // ServiceWorker passed to respondWith() if it did. Otherwise returns an empty
  // URL.
  BLINK_PLATFORM_EXPORT WebURL OriginalURLViaServiceWorker() const;

  // The boundary of the response. Set only when this is a multipart response.
  BLINK_PLATFORM_EXPORT void SetMultipartBoundary(const char* bytes,
                                                  size_t /* size */);

  // The cache name of the CacheStorage from where the response is served via
  // the ServiceWorker. Null if the response isn't from the CacheStorage.
  BLINK_PLATFORM_EXPORT void SetCacheStorageCacheName(const WebString&);

  // The headers that should be exposed according to CORS. Only guaranteed
  // to be set if the response was served by a ServiceWorker.
  BLINK_PLATFORM_EXPORT WebVector<WebString> CorsExposedHeaderNames() const;
  BLINK_PLATFORM_EXPORT void SetCorsExposedHeaderNames(
      const WebVector<WebString>&);

  // Whether service worker navigation preload occurred.
  // See ServiceWorkerResponseInfo::did_navigation_preload() for
  // details.
  BLINK_PLATFORM_EXPORT void SetDidServiceWorkerNavigationPreload(bool);

  // Remote IP address of the socket which fetched this resource.
  BLINK_PLATFORM_EXPORT WebString RemoteIPAddress() const;
  BLINK_PLATFORM_EXPORT void SetRemoteIPAddress(const WebString&);

  // Remote port number of the socket which fetched this resource.
  BLINK_PLATFORM_EXPORT unsigned short RemotePort() const;
  BLINK_PLATFORM_EXPORT void SetRemotePort(unsigned short);

  // ALPN negotiated protocol of the socket which fetched this resource.
  BLINK_PLATFORM_EXPORT WebString AlpnNegotiatedProtocol() const;
  BLINK_PLATFORM_EXPORT void SetAlpnNegotiatedProtocol(const WebString&);

  // Information about the type of connection used to fetch this resource.
  BLINK_PLATFORM_EXPORT net::HttpResponseInfo::ConnectionInfo ConnectionInfo()
      const;
  BLINK_PLATFORM_EXPORT void SetConnectionInfo(
      net::HttpResponseInfo::ConnectionInfo);

  // Original size of the response before decompression.
  BLINK_PLATFORM_EXPORT void SetEncodedDataLength(long long);

  BLINK_PLATFORM_EXPORT void SetIsSignedExchangeInnerResponse(bool);

  // Extra data associated with the underlying resource response. Resource
  // responses can be copied. If non-null, each copy of a resource response
  // holds a pointer to the extra data, and the extra data pointer will be
  // deleted when the last resource response is destroyed. Setting the extra
  // data pointer will cause the underlying resource response to be
  // dissociated from any existing non-null extra data pointer.
  BLINK_PLATFORM_EXPORT ExtraData* GetExtraData() const;
  BLINK_PLATFORM_EXPORT void SetExtraData(ExtraData*);

  BLINK_PLATFORM_EXPORT void AppendRedirectResponse(const WebURLResponse&);

#if INSIDE_BLINK
 protected:
  // Permit subclasses to set arbitrary ResourceResponse pointer as
  // |resource_response_|. |owned_resource_response_| is not set in this case.
  BLINK_PLATFORM_EXPORT explicit WebURLResponse(ResourceResponse&);
#endif

 private:
  // If this instance owns a ResourceResponse then |owned_resource_response_|
  // is non-null and |resource_response_| points to the ResourceResponse
  // instance it contains.
  const std::unique_ptr<ResourceResponse> owned_resource_response_;

  // Should never be null.
  ResourceResponse* const resource_response_;
};

}  // namespace blink

#endif
