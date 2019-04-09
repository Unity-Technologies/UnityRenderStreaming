/*
 * Copyright (C) 2006, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_LOADER_FETCH_RESOURCE_RESPONSE_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_LOADER_FETCH_RESOURCE_RESPONSE_H_

#include <memory>
#include <utility>

#include "base/memory/scoped_refptr.h"
#include "base/time/time.h"
#include "services/network/public/cpp/cors/cors.h"
#include "services/network/public/mojom/fetch_api.mojom-blink.h"
#include "third_party/blink/public/platform/web_url_response.h"
#include "third_party/blink/renderer/platform/blob/blob_data.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource_load_info.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource_load_timing.h"
#include "third_party/blink/renderer/platform/network/http_header_map.h"
#include "third_party/blink/renderer/platform/network/http_parsers.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/wtf/noncopyable.h"
#include "third_party/blink/renderer/platform/wtf/ref_counted.h"
#include "third_party/blink/renderer/platform/wtf/text/cstring.h"
#include "third_party/blink/renderer/platform/wtf/time.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

// A ResourceResponse is a "response" object used in blink. Conceptually
// it is https://fetch.spec.whatwg.org/#concept-response, but it contains
// a lot of blink specific fields. WebURLResponse is the "public version"
// of this class and public classes (i.e., classes in public/platform) use it.
//
// This class is thread-bound. Do not copy/pass an instance across threads.
class PLATFORM_EXPORT ResourceResponse final {
 public:
  enum HTTPVersion : uint8_t {
    kHTTPVersionUnknown,
    kHTTPVersion_0_9,
    kHTTPVersion_1_0,
    kHTTPVersion_1_1,
    kHTTPVersion_2_0
  };
  enum SecurityStyle : uint8_t {
    kSecurityStyleUnknown,
    kSecurityStyleUnauthenticated,
    kSecurityStyleAuthenticationBroken,
    kSecurityStyleAuthenticated
  };

  enum CTPolicyCompliance {
    kCTPolicyComplianceDetailsNotAvailable,
    kCTPolicyComplies,
    kCTPolicyDoesNotComply
  };

  class PLATFORM_EXPORT SignedCertificateTimestamp final {
   public:
    SignedCertificateTimestamp(String status,
                               String origin,
                               String log_description,
                               String log_id,
                               int64_t timestamp,
                               String hash_algorithm,
                               String signature_algorithm,
                               String signature_data)
        : status_(status),
          origin_(origin),
          log_description_(log_description),
          log_id_(log_id),
          timestamp_(timestamp),
          hash_algorithm_(hash_algorithm),
          signature_algorithm_(signature_algorithm),
          signature_data_(signature_data) {}
    explicit SignedCertificateTimestamp(
        const struct blink::WebURLResponse::SignedCertificateTimestamp&);
    SignedCertificateTimestamp IsolatedCopy() const;

    String status_;
    String origin_;
    String log_description_;
    String log_id_;
    int64_t timestamp_;
    String hash_algorithm_;
    String signature_algorithm_;
    String signature_data_;
  };

  using SignedCertificateTimestampList =
      WTF::Vector<SignedCertificateTimestamp>;

  struct SecurityDetails {
    DISALLOW_NEW();
    SecurityDetails() : valid_from(0), valid_to(0) {}
    // All strings are human-readable values.
    String protocol;
    // keyExchange is the empty string if not applicable for the connection's
    // protocol.
    String key_exchange;
    // keyExchangeGroup is the empty string if not applicable for the
    // connection's key exchange.
    String key_exchange_group;
    String cipher;
    // mac is the empty string when the connection cipher suite does not
    // have a separate MAC value (i.e. if the cipher suite is AEAD).
    String mac;
    String subject_name;
    Vector<String> san_list;
    String issuer;
    time_t valid_from;
    time_t valid_to;
    // DER-encoded X509Certificate certificate chain.
    Vector<AtomicString> certificate;
    SignedCertificateTimestampList sct_list;
  };

  class ExtraData : public RefCounted<ExtraData> {
   public:
    virtual ~ExtraData() = default;
  };

  ResourceResponse();
  explicit ResourceResponse(const KURL&);
  ResourceResponse(const ResourceResponse&);
  ResourceResponse& operator=(const ResourceResponse&);

  bool IsNull() const { return is_null_; }
  bool IsHTTP() const;

  // The URL of the resource. Note that if a service worker responded to the
  // request for this resource, it may have fetched an entirely different URL
  // and responded with that resource. wasFetchedViaServiceWorker() and
  // originalURLViaServiceWorker() can be used to determine whether and how a
  // service worker responded to the request. Example service worker code:
  //
  // onfetch = (event => {
  //   if (event.request.url == 'https://abc.com')
  //     event.respondWith(fetch('https://def.com'));
  // });
  //
  // If this service worker responds to an "https://abc.com" request, then for
  // the resulting ResourceResponse, url() is "https://abc.com",
  // wasFetchedViaServiceWorker() is true, and originalURLViaServiceWorker() is
  // "https://def.com".
  const KURL& Url() const;
  void SetURL(const KURL&);

  const AtomicString& MimeType() const;
  void SetMimeType(const AtomicString&);

  int64_t ExpectedContentLength() const;
  void SetExpectedContentLength(int64_t);

  const AtomicString& TextEncodingName() const;
  void SetTextEncodingName(const AtomicString&);

  int HttpStatusCode() const;
  void SetHTTPStatusCode(int);

  const AtomicString& HttpStatusText() const;
  void SetHTTPStatusText(const AtomicString&);

  const AtomicString& HttpHeaderField(const AtomicString& name) const;
  void SetHTTPHeaderField(const AtomicString& name, const AtomicString& value);
  void AddHTTPHeaderField(const AtomicString& name, const AtomicString& value);
  void ClearHTTPHeaderField(const AtomicString& name);
  const HTTPHeaderMap& HttpHeaderFields() const;

  bool IsMultipart() const { return MimeType() == "multipart/x-mixed-replace"; }

  bool IsAttachment() const;

  AtomicString HttpContentType() const;

  // These functions return parsed values of the corresponding response headers.
  // NaN means that the header was not present or had invalid value.
  bool CacheControlContainsNoCache() const;
  bool CacheControlContainsNoStore() const;
  bool CacheControlContainsMustRevalidate() const;
  bool HasCacheValidatorFields() const;
  double CacheControlMaxAge() const;
  double Date() const;
  double Age() const;
  double Expires() const;
  double LastModified() const;
  // Will always return values >= 0.
  double CacheControlStaleWhileRevalidate() const;

  unsigned ConnectionID() const;
  void SetConnectionID(unsigned);

  bool ConnectionReused() const;
  void SetConnectionReused(bool);

  bool WasCached() const;
  void SetWasCached(bool);

  ResourceLoadTiming* GetResourceLoadTiming() const;
  void SetResourceLoadTiming(scoped_refptr<ResourceLoadTiming>);

  scoped_refptr<ResourceLoadInfo> GetResourceLoadInfo() const;
  void SetResourceLoadInfo(scoped_refptr<ResourceLoadInfo>);

  HTTPVersion HttpVersion() const { return http_version_; }
  void SetHTTPVersion(HTTPVersion version) { http_version_ = version; }

  int RequestId() const { return request_id_; }
  void SetRequestId(int request_id) { request_id_ = request_id; }

  bool HasMajorCertificateErrors() const {
    return has_major_certificate_errors_;
  }
  void SetHasMajorCertificateErrors(bool has_major_certificate_errors) {
    has_major_certificate_errors_ = has_major_certificate_errors;
  }

  CTPolicyCompliance GetCTPolicyCompliance() const {
    return ct_policy_compliance_;
  }
  void SetCTPolicyCompliance(CTPolicyCompliance);

  bool IsLegacySymantecCert() const { return is_legacy_symantec_cert_; }
  void SetIsLegacySymantecCert(bool is_legacy_symantec_cert) {
    is_legacy_symantec_cert_ = is_legacy_symantec_cert;
  }

  bool IsLegacyTLSVersion() const { return is_legacy_tls_version_; }
  void SetIsLegacyTLSVersion(bool value) { is_legacy_tls_version_ = value; }

  SecurityStyle GetSecurityStyle() const { return security_style_; }
  void SetSecurityStyle(SecurityStyle security_style) {
    security_style_ = security_style;
  }

  const SecurityDetails* GetSecurityDetails() const {
    return &security_details_;
  }
  void SetSecurityDetails(const String& protocol,
                          const String& key_exchange,
                          const String& key_exchange_group,
                          const String& cipher,
                          const String& mac,
                          const String& subject_name,
                          const Vector<String>& san_list,
                          const String& issuer,
                          time_t valid_from,
                          time_t valid_to,
                          const Vector<AtomicString>& certificate,
                          const SignedCertificateTimestampList& sct_list);

  int64_t AppCacheID() const { return app_cache_id_; }
  void SetAppCacheID(int64_t id) { app_cache_id_ = id; }

  const KURL& AppCacheManifestURL() const { return app_cache_manifest_url_; }
  void SetAppCacheManifestURL(const KURL& url) {
    app_cache_manifest_url_ = url;
  }

  bool WasFetchedViaSPDY() const { return was_fetched_via_spdy_; }
  void SetWasFetchedViaSPDY(bool value) { was_fetched_via_spdy_ = value; }

  // See ServiceWorkerResponseInfo::was_fetched_via_service_worker.
  bool WasFetchedViaServiceWorker() const {
    return was_fetched_via_service_worker_;
  }
  void SetWasFetchedViaServiceWorker(bool value) {
    was_fetched_via_service_worker_ = value;
  }

  // See ServiceWorkerResponseInfo::was_fallback_required.
  bool WasFallbackRequiredByServiceWorker() const {
    return was_fallback_required_by_service_worker_;
  }
  void SetWasFallbackRequiredByServiceWorker(bool value) {
    was_fallback_required_by_service_worker_ = value;
  }

  network::mojom::FetchResponseType GetType() const { return response_type_; }
  void SetType(network::mojom::FetchResponseType value) {
    response_type_ = value;
  }
  bool IsOpaqueResponseFromServiceWorker() const;
  // https://html.spec.whatwg.org/#cors-same-origin
  bool IsCorsSameOrigin() const {
    return network::cors::IsCorsSameOriginResponseType(response_type_);
  }
  // https://html.spec.whatwg.org/#cors-cross-origin
  bool IsCorsCrossOrigin() const {
    return network::cors::IsCorsCrossOriginResponseType(response_type_);
  }

  // See ServiceWorkerResponseInfo::url_list_via_service_worker.
  const Vector<KURL>& UrlListViaServiceWorker() const {
    return url_list_via_service_worker_;
  }
  void SetURLListViaServiceWorker(const Vector<KURL>& url_list) {
    url_list_via_service_worker_ = url_list;
  }

  // Returns the last URL of urlListViaServiceWorker if exists. Otherwise
  // returns an empty URL.
  KURL OriginalURLViaServiceWorker() const;

  const Vector<char>& MultipartBoundary() const { return multipart_boundary_; }
  void SetMultipartBoundary(const char* bytes, uint32_t size) {
    multipart_boundary_.clear();
    multipart_boundary_.Append(bytes, size);
  }

  const String& CacheStorageCacheName() const {
    return cache_storage_cache_name_;
  }
  void SetCacheStorageCacheName(const String& cache_storage_cache_name) {
    cache_storage_cache_name_ = cache_storage_cache_name;
  }

  const Vector<String>& CorsExposedHeaderNames() const {
    return cors_exposed_header_names_;
  }
  void SetCorsExposedHeaderNames(const Vector<String>& header_names) {
    cors_exposed_header_names_ = header_names;
  }

  bool DidServiceWorkerNavigationPreload() const {
    return did_service_worker_navigation_preload_;
  }
  void SetDidServiceWorkerNavigationPreload(bool value) {
    did_service_worker_navigation_preload_ = value;
  }

  Time ResponseTime() const { return response_time_; }
  void SetResponseTime(Time response_time) { response_time_ = response_time; }

  const AtomicString& RemoteIPAddress() const { return remote_ip_address_; }
  void SetRemoteIPAddress(const AtomicString& value) {
    remote_ip_address_ = value;
  }

  unsigned short RemotePort() const { return remote_port_; }
  void SetRemotePort(unsigned short value) { remote_port_ = value; }

  const AtomicString& AlpnNegotiatedProtocol() const {
    return alpn_negotiated_protocol_;
  }
  void SetAlpnNegotiatedProtocol(const AtomicString& value) {
    alpn_negotiated_protocol_ = value;
  }

  net::HttpResponseInfo::ConnectionInfo ConnectionInfo() const {
    return connection_info_;
  }
  void SetConnectionInfo(net::HttpResponseInfo::ConnectionInfo value) {
    connection_info_ = value;
  }

  AtomicString ConnectionInfoString() const;

  int64_t EncodedDataLength() const { return encoded_data_length_; }
  void SetEncodedDataLength(int64_t value);

  int64_t EncodedBodyLength() const { return encoded_body_length_; }
  void SetEncodedBodyLength(int64_t value);

  int64_t DecodedBodyLength() const { return decoded_body_length_; }
  void SetDecodedBodyLength(int64_t value);

  // Extra data associated with this response.
  ExtraData* GetExtraData() const { return extra_data_.get(); }
  void SetExtraData(scoped_refptr<ExtraData> extra_data) {
    extra_data_ = std::move(extra_data);
  }

  unsigned MemoryUsage() const {
    // average size, mostly due to URL and Header Map strings
    return 1280;
  }

  // PlzNavigate: Even if there is redirections, only one
  // ResourceResponse is built: the final response.
  // The redirect response chain can be accessed by this function.
  const Vector<ResourceResponse>& RedirectResponses() const {
    return redirect_responses_;
  }
  void AppendRedirectResponse(const ResourceResponse&);

  bool AsyncRevalidationRequested() const {
    return async_revalidation_requested_;
  }

  void SetAsyncRevalidationRequested(bool requested) {
    async_revalidation_requested_ = requested;
  }

  bool NetworkAccessed() const { return network_accessed_; }

  void SetNetworkAccessed(bool network_accessed) {
    network_accessed_ = network_accessed;
  }

  bool IsSignedExchangeInnerResponse() const {
    return is_signed_exchange_inner_response_;
  }

  void SetIsSignedExchangeInnerResponse(
      bool is_signed_exchange_inner_response) {
    is_signed_exchange_inner_response_ = is_signed_exchange_inner_response;
  }

  // This method doesn't compare the all members.
  static bool Compare(const ResourceResponse&, const ResourceResponse&);

 private:
  void UpdateHeaderParsedState(const AtomicString& name);

  KURL url_;
  AtomicString mime_type_;
  long long expected_content_length_ = 0;
  AtomicString text_encoding_name_;

  unsigned connection_id_ = 0;
  int http_status_code_ = 0;
  AtomicString http_status_text_;
  HTTPHeaderMap http_header_fields_;

  // Remote IP address of the socket which fetched this resource.
  AtomicString remote_ip_address_;

  // Remote port number of the socket which fetched this resource.
  unsigned short remote_port_ = 0;

  bool was_cached_ = false;
  bool connection_reused_ = false;
  bool is_null_;
  mutable bool have_parsed_age_header_ = false;
  mutable bool have_parsed_date_header_ = false;
  mutable bool have_parsed_expires_header_ = false;
  mutable bool have_parsed_last_modified_header_ = false;

  // True if the resource was retrieved by the embedder in spite of
  // certificate errors.
  bool has_major_certificate_errors_ = false;

  // The Certificate Transparency policy compliance status of the resource.
  CTPolicyCompliance ct_policy_compliance_ =
      kCTPolicyComplianceDetailsNotAvailable;

  // True if the resource was retrieved with a legacy Symantec certificate which
  // is slated for distrust in future.
  bool is_legacy_symantec_cert_ = false;

  // True if the response was sent over TLS 1.0 or 1.1, which are deprecated and
  // will be removed in the future.
  bool is_legacy_tls_version_ = false;

  // The time at which the resource's certificate expires. Null if there was no
  // certificate.
  base::Time cert_validity_start_;

  // Was the resource fetched over SPDY.  See http://dev.chromium.org/spdy
  bool was_fetched_via_spdy_ = false;

  // Was the resource fetched over a ServiceWorker.
  bool was_fetched_via_service_worker_ = false;

  // Was the fallback request with skip service worker flag required.
  bool was_fallback_required_by_service_worker_ = false;

  // True if service worker navigation preload was performed due to
  // the request for this resource.
  bool did_service_worker_navigation_preload_ = false;

  // True if this resource is stale and needs async revalidation. Will only
  // possibly be set if the load_flags indicated SUPPORT_ASYNC_REVALIDATION.
  bool async_revalidation_requested_ = false;

  // True if this resource is from an inner response of a signed exchange.
  // https://wicg.github.io/webpackage/draft-yasskin-http-origin-signed-responses.html
  bool is_signed_exchange_inner_response_ = false;

  // True if this resource was loaded from the network.
  bool network_accessed_ = false;

  // https://fetch.spec.whatwg.org/#concept-response-type
  network::mojom::FetchResponseType response_type_ =
      network::mojom::FetchResponseType::kDefault;

  // HTTP version used in the response, if known.
  HTTPVersion http_version_ = kHTTPVersionUnknown;

  // Request id given to the resource by the WebUrlLoader.
  int request_id_ = 0;

  // The security style of the resource.
  // This only contains a valid value when the DevTools Network domain is
  // enabled. (Otherwise, it contains a default value of Unknown.)
  SecurityStyle security_style_ = kSecurityStyleUnknown;

  // Security details of this request's connection.
  // If m_securityStyle is Unknown or Unauthenticated, this does not contain
  // valid data.
  SecurityDetails security_details_;

  scoped_refptr<ResourceLoadTiming> resource_load_timing_;
  scoped_refptr<ResourceLoadInfo> resource_load_info_;

  mutable CacheControlHeader cache_control_header_;

  mutable double age_ = 0.0;
  mutable double date_ = 0.0;
  mutable double expires_ = 0.0;
  mutable double last_modified_ = 0.0;

  // The id of the appcache this response was retrieved from, or zero if
  // the response was not retrieved from an appcache.
  int64_t app_cache_id_ = 0;

  // The manifest url of the appcache this response was retrieved from, if any.
  // Note: only valid for main resource responses.
  KURL app_cache_manifest_url_;

  // The multipart boundary of this response.
  Vector<char> multipart_boundary_;

  // The URL list of the response which was fetched by the ServiceWorker.
  // This is empty if the response was created inside the ServiceWorker.
  Vector<KURL> url_list_via_service_worker_;

  // The cache name of the CacheStorage from where the response is served via
  // the ServiceWorker. Null if the response isn't from the CacheStorage.
  String cache_storage_cache_name_;

  // The headers that should be exposed according to CORS. Only guaranteed
  // to be set if the response was fetched by a ServiceWorker.
  Vector<String> cors_exposed_header_names_;

  // The time at which the response headers were received.  For cached
  // responses, this time could be "far" in the past.
  Time response_time_;

  // ALPN negotiated protocol of the socket which fetched this resource.
  AtomicString alpn_negotiated_protocol_;

  // Information about the type of connection used to fetch this resource.
  net::HttpResponseInfo::ConnectionInfo connection_info_ =
      net::HttpResponseInfo::ConnectionInfo::CONNECTION_INFO_UNKNOWN;

  // Size of the response in bytes prior to decompression.
  int64_t encoded_data_length_ = 0;

  // Size of the response body in bytes prior to decompression.
  int64_t encoded_body_length_ = 0;

  // Sizes of the response body in bytes after any content-encoding is
  // removed.
  int64_t decoded_body_length_ = 0;

  // ExtraData associated with the response.
  scoped_refptr<ExtraData> extra_data_;

  // PlzNavigate: the redirect responses are transmitted
  // inside the final response.
  Vector<ResourceResponse> redirect_responses_;
};

inline bool operator==(const ResourceResponse& a, const ResourceResponse& b) {
  return ResourceResponse::Compare(a, b);
}
inline bool operator!=(const ResourceResponse& a, const ResourceResponse& b) {
  return !(a == b);
}

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_LOADER_FETCH_RESOURCE_RESPONSE_H_
