/*
 * Copyright (C) 2007, 2008 Apple Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_WEBORIGIN_SECURITY_ORIGIN_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_WEBORIGIN_SECURITY_ORIGIN_H_

#include <stdint.h>
#include <memory>

#include "base/gtest_prod_util.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/noncopyable.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/platform/wtf/thread_safe_ref_counted.h"
#include "url/origin.h"

namespace mojo {
struct UrlOriginAdapter;
}  // namespace mojo

namespace blink {

class KURL;
class URLSecurityOriginMap;

// An identifier which defines the source of content (e.g. a document) and
// restricts what other objects it is permitted to access (based on their
// security origin). Most commonly, an origin is a (scheme, host, port, domain)
// tuple, such as the tuple origin (https, chromium.org, null, null). However,
// there are also opaque origins which do not have a corresponding tuple.
//
// See also: https://html.spec.whatwg.org/multipage/origin.html#concept-origin
class PLATFORM_EXPORT SecurityOrigin : public RefCounted<SecurityOrigin> {
  WTF_MAKE_NONCOPYABLE(SecurityOrigin);

 public:
  enum class AccessResultDomainDetail {
    kDomainNotRelevant,
    kDomainNotSet,
    kDomainSetByOnlyOneOrigin,
    kDomainMatchNecessary,
    kDomainMatchUnnecessary,
    kDomainMismatch,
  };

  // SecurityOrigin::Create() resolves |url| to its SecurityOrigin. When |url|
  // contains a standard (scheme, host, port) tuple, |reference_origin| is
  // ignored. If |reference_origin| is provided and an opaque origin is returned
  // (for example, if |url| has the "data:" scheme), the opaque origin will be
  // derived from |reference_origin|, retaining the precursor information.
  static scoped_refptr<SecurityOrigin> CreateWithReferenceOrigin(
      const KURL& url,
      const SecurityOrigin* reference_origin);

  // Equivalent to CreateWithReferenceOrigin without supplying value for
  // |reference_origin|.
  static scoped_refptr<SecurityOrigin> Create(const KURL& url);

  // Creates a new opaque SecurityOrigin that is guaranteed to be cross-origin
  // to all currently existing SecurityOrigins.
  static scoped_refptr<SecurityOrigin> CreateUniqueOpaque();

  static scoped_refptr<SecurityOrigin> CreateFromString(const String&);
  static scoped_refptr<SecurityOrigin> Create(const String& protocol,
                                              const String& host,
                                              uint16_t port);
  static scoped_refptr<SecurityOrigin> CreateFromUrlOrigin(const url::Origin&);
  url::Origin ToUrlOrigin() const;

  static void SetMap(URLSecurityOriginMap*);

  // Some URL schemes use nested URLs for their security context. For example,
  // filesystem URLs look like the following:
  //
  //   filesystem:http://example.com/temporary/path/to/file.png
  //
  // We're supposed to use "http://example.com" as the origin.
  //
  // Generally, we add URL schemes to this list when Blink supports them. For
  // example, we don't include the "jar" scheme, even though Firefox
  // understands that "jar" uses an inner URL for its security origin.
  static bool ShouldUseInnerURL(const KURL&);
  static KURL ExtractInnerURL(const KURL&);

  // Create a deep copy of this SecurityOrigin. This method is useful
  // when marshalling a SecurityOrigin to another thread.
  scoped_refptr<SecurityOrigin> IsolatedCopy() const;

  // Set the domain property of this security origin to newDomain. This
  // function does not check whether newDomain is a suffix of the current
  // domain. The caller is responsible for validating newDomain.
  void SetDomainFromDOM(const String& new_domain);
  bool DomainWasSetInDOM() const { return domain_was_set_in_dom_; }

  String Protocol() const { return protocol_; }
  String Host() const { return host_; }
  String Domain() const { return domain_; }

  // Returns 0 if the effective port of this origin is the default for its
  // scheme.
  uint16_t Port() const { return port_; }
  // Returns the effective port, even if it is the default port for the
  // scheme (e.g. "http" => 80).
  uint16_t EffectivePort() const { return effective_port_; }

  // Returns true if a given URL is secure, based either directly on its
  // own protocol, or, when relevant, on the protocol of its "inner URL"
  // Protocols like blob: and filesystem: fall into this latter category.
  static bool IsSecure(const KURL&);

  // Returns true if this SecurityOrigin can script objects in the given
  // SecurityOrigin. For example, call this function before allowing
  // script from one security origin to read or write objects from
  // another SecurityOrigin.
  bool CanAccess(const SecurityOrigin* other) const {
    AccessResultDomainDetail unused_detail;
    return CanAccess(other, unused_detail);
  }

  // Returns true if this SecurityOrigin can script objects in |other|, just
  // as above, but also returns the category into which the access check fell.
  //
  // TODO(crbug.com/787905): Remove this variant once we have enough data to
  // make decisions about `document.domain`.
  bool CanAccess(const SecurityOrigin* other, AccessResultDomainDetail&) const;

  // Returns true if this SecurityOrigin can read content retrieved from
  // the given URL.
  // Note: This function may return false when |url| has data scheme, which
  // is not aligned with CORS. If you want a CORS-aligned check, just use
  // CORS mode (e.g., network::mojom::FetchRequestMode::kSameOrigin), or
  // use CanReadContent.
  // See
  // https://docs.google.com/document/d/1_BD15unoPJVwKyf5yOUDu5kie492TTaBxzhJ58j1rD4/edit.
  bool CanRequest(const KURL& url) const;

  // Returns true if content from this URL can be read without CORS from this
  // security origin. For example, call this function before drawing an image
  // onto an HTML canvas element with the drawImage API.
  bool CanReadContent(const KURL&) const;

  // Returns true if |document| can display content from the given URL (e.g.,
  // in an iframe or as an image). For example, web sites generally cannot
  // display content from the user's files system.
  bool CanDisplay(const KURL&) const;

  // Returns true if the origin loads resources either from the local
  // machine or over the network from a
  // cryptographically-authenticated origin, as described in
  // https://w3c.github.io/webappsec-secure-contexts/#is-origin-trustworthy
  bool IsPotentiallyTrustworthy() const;

  // Returns a human-readable error message describing that a non-secure
  // origin's access to a feature is denied.
  static String IsPotentiallyTrustworthyErrorMessage();

  // Returns true if this SecurityOrigin can load local resources, such
  // as images, iframes, and style sheets, and can link to local URLs.
  // For example, call this function before creating an iframe to a
  // file:// URL.
  //
  // Note: A SecurityOrigin might be allowed to load local resources
  //       without being able to issue an XMLHttpRequest for a local URL.
  //       To determine whether the SecurityOrigin can issue an
  //       XMLHttpRequest for a URL, call canReadContent(url).
  bool CanLoadLocalResources() const { return can_load_local_resources_; }

  // Explicitly grant the ability to load local resources to this
  // SecurityOrigin.
  //
  // Note: This method exists only to support backwards compatibility
  //       with older versions of WebKit.
  void GrantLoadLocalResources();

  // Explicitly grant the ability to access every other SecurityOrigin.
  //
  // WARNING: This is an extremely powerful ability. Use with caution!
  void GrantUniversalAccess();
  bool IsGrantedUniversalAccess() const { return universal_access_; }

  bool CanAccessDatabase() const { return !IsOpaque(); }
  bool CanAccessLocalStorage() const { return !IsOpaque(); }
  bool CanAccessSharedWorkers() const { return !IsOpaque(); }
  bool CanAccessServiceWorkers() const { return !IsOpaque(); }
  bool CanAccessCookies() const { return !IsOpaque(); }
  bool CanAccessPasswordManager() const { return !IsOpaque(); }
  bool CanAccessFileSystem() const { return !IsOpaque(); }
  bool CanAccessCacheStorage() const { return !IsOpaque(); }
  bool CanAccessLocks() const { return !IsOpaque(); }

  // Technically, we should always allow access to sessionStorage, but we
  // currently don't handle creating a sessionStorage area for opaque
  // origins.
  bool CanAccessSessionStorage() const { return !IsOpaque(); }

  // The local SecurityOrigin is the most privileged SecurityOrigin.
  // The local SecurityOrigin can script any document, navigate to local
  // resources, and can set arbitrary headers on XMLHttpRequests.
  bool IsLocal() const;

  // Returns true if the host is one of 127.0.0.1/8, ::1/128, or "localhost".
  bool IsLocalhost() const;

  // Returns true if the origin is not a tuple origin (i.e. an origin consisting
  // of a scheme, host, port, and domain). Opaque origins are created for a
  // variety of situations (see https://whatwg.org/C/origin.html#origin for more
  // details), such as for documents generated from data: URLs or documents
  // with the sandboxed origin browsing context flag set.
  bool IsOpaque() const { return !!nonce_if_opaque_; }

  // By default 'file:' URLs may access other 'file:' URLs. This method
  // denies access. If either SecurityOrigin sets this flag, the access
  // check will fail.
  void BlockLocalAccessFromLocalOrigin();

  // Convert this SecurityOrigin into a string. The string representation of a
  // SecurityOrigin is similar to a URL, except it lacks a path component. The
  // string representation does not encode the value of the SecurityOrigin's
  // domain property.
  //
  // When using the string value, it's important to remember that it might be
  // "null". This typically happens when this SecurityOrigin is opaque (e.g. the
  // origin of a sandboxed iframe).
  String ToString() const;
  AtomicString ToAtomicString() const;

  // Similar to ToString(), but does not take into account any factors that
  // could make the string return "null".
  String ToRawString() const;

  // Returns a token that helps distinguish origins, or null string. When not
  // null string, the tokens are guaranteed to be different if not the same
  // origin, i.e. if two tokens are the same and not null, the two
  // SecurityOrigins are the same origin. Thus, tokens can be used for fast
  // check of origins.
  //
  // This is pretty similar to ToString(), but this returns null string instead
  // of "null", and includes a host part in case of file: scheme.
  //
  // Note that the same tokens only guarantee that the SecurityOrigins are
  // the same origin and not the same origin-domain. See also:
  // https://html.spec.whatwg.org/C/origin.html#same-origin
  // https://html.spec.whatwg.org/C/origin.html#same-origin-domain
  String ToTokenForFastCheck() const;

  // This method checks for equality, ignoring the value of document.domain
  // (and whether it was set) but considering the host. It is used for
  // postMessage.
  bool IsSameSchemeHostPort(const SecurityOrigin*) const;

  static bool AreSameSchemeHostPort(const KURL& a, const KURL& b);

  static const KURL& UrlWithUniqueOpaqueOrigin();

  // Transfer origin privileges from another security origin.
  // The following privileges are currently copied over:
  //
  //   - Grant universal access.
  //   - Grant loading of local resources.
  //   - Use path-based file:// origins.
  struct PrivilegeData {
    bool universal_access_;
    bool can_load_local_resources_;
    bool block_local_access_from_local_origin_;
  };
  std::unique_ptr<PrivilegeData> CreatePrivilegeData() const;
  void TransferPrivilegesFrom(std::unique_ptr<PrivilegeData>);

  void SetOpaqueOriginIsPotentiallyTrustworthy(
      bool is_opaque_origin_potentially_trustworthy);

  // Creates a new opaque security origin derived from |this| (|this| becomes
  // its precursor).
  scoped_refptr<SecurityOrigin> DeriveNewOpaqueOrigin() const;

  // Only used for document.domain setting. The method should probably be moved
  // if we need it for something more general.
  static String CanonicalizeHost(const String& host, bool* success);

 private:
  constexpr static const int kInvalidPort = 0;

  friend struct mojo::UrlOriginAdapter;

  // Creates a new opaque SecurityOrigin using the supplied |precursor| origin
  // and |nonce|.
  static scoped_refptr<SecurityOrigin> CreateOpaque(
      const url::Origin::Nonce& nonce,
      const SecurityOrigin* precursor);

  // Create an opaque SecurityOrigin.
  SecurityOrigin(const url::Origin::Nonce& nonce,
                 const SecurityOrigin* precursor_origin);

  // Create a tuple SecurityOrigin, with parameters via KURL
  explicit SecurityOrigin(const KURL& url);

  // Clone a SecurityOrigin which is safe to use on other threads.
  explicit SecurityOrigin(const SecurityOrigin* other);

  // FIXME: Rename this function to something more semantic.
  bool PassesFileCheck(const SecurityOrigin*) const;
  void BuildRawString(StringBuilder&) const;

  bool SerializesAsNull() const;

  // Get the nonce associated with this origin, if it is unique. This should be
  // used only when trying to send an Origin across an IPC pipe.
  base::Optional<base::UnguessableToken> GetNonceForSerialization() const;

  // If this is an opaque origin that was derived from a tuple origin, return
  // the origin from which this was derived. Otherwise returns |this|.
  const SecurityOrigin* GetOriginOrPrecursorOriginIfOpaque() const;

  const String protocol_ = g_empty_string;
  const String host_ = g_empty_string;
  String domain_ = g_empty_string;
  const uint16_t port_ = kInvalidPort;
  const uint16_t effective_port_ = kInvalidPort;
  const base::Optional<url::Origin::Nonce> nonce_if_opaque_;
  bool universal_access_ = false;
  bool domain_was_set_in_dom_ = false;
  bool can_load_local_resources_ = false;
  bool block_local_access_from_local_origin_ = false;
  bool is_opaque_origin_potentially_trustworthy_ = false;

  // For opaque origins, tracks the non-opaque origin from which the opaque
  // origin is derived.
  const scoped_refptr<const SecurityOrigin> precursor_origin_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_WEBORIGIN_SECURITY_ORIGIN_H_
