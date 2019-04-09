/*
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2011, 2012 Apple Inc.
 * All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_WEBORIGIN_KURL_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_WEBORIGIN_KURL_H_

#include <memory>
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "url/third_party/mozilla/url_parse.h"
#include "url/url_canon.h"
#include "url/url_util.h"

// KURL stands for the URL parser in KDE's HTML Widget (KHTML). The name hasn't
// changed since Blink forked WebKit, which in turn forked KHTML.
//
// KURL is Blink's main URL class, and is the analog to GURL in other Chromium
// code. It is not thread safe but is generally cheap to copy and compare KURLs
// to each other.
//
// KURL and GURL both share the same underlying URL parser, whose code is
// located in //url, but KURL is backed by Blink specific WTF::Strings. This
// means that KURLs are usually cheap to copy due to WTF::Strings being
// internally ref-counted. However, please don't copy KURLs if you can use a
// const ref, since the size of the parsed structure and related metadata is
// non-trivial.
//
// In fact, for the majority of KURLs (i.e. those not copied across threads),
// the backing string is an AtomicString, meaning that it is stored in the
// thread-local AtomicString table, allowing optimizations like fast comparison.
// See platform/wtf/text/AtomicString.h for information on the performance
// characteristics of AtomicStrings.
//
// KURL also has a few other optimizations, including:
//  - Cached bit for whether the KURL is http/https
//  - Internal reference to the URL protocol (scheme) to avoid String allocation
//    for the callers that require it. Common protocols like http and https are
//    stored as static strings which can be shared across threads.
namespace WTF {
class TextEncoding;
}

class GURL;

namespace blink {

struct KURLHash;

class PLATFORM_EXPORT KURL {
  USING_FAST_MALLOC(KURL);

 public:
  // This must be called during initialization (before we create
  // other threads).
  static void Initialize();

  KURL();
  KURL(const KURL&);

  // This should only be used to convert a GURL returned from a layer which
  // operates in the base terms (e.g. from common/* code).
  explicit KURL(const GURL&);

  KURL& operator=(const KURL&);

  // The argument is an absolute URL string. The string is assumed to be
  // output of KURL::GetString() called on a valid KURL object, or
  // indiscernible from such.
  //
  // It is usually best to avoid repeatedly parsing a String, unless memory
  // saving outweigh the possible slow-downs.
  explicit KURL(const String&);

  // Creates an isolated URL object suitable for sending to another thread.
  static KURL CreateIsolated(const String&);

  // Resolves the relative URL with the given base URL. If provided, the
  // TextEncoding is used to encode non-ASCII characters. The base URL can be
  // null or empty, in which case the relative URL will be interpreted as
  // absolute.
  // FIXME: If the base URL is invalid, this always creates an invalid
  // URL. Instead I think it would be better to treat all invalid base URLs
  // the same way we treate null and empty base URLs.
  KURL(const KURL& base, const String& relative);
  KURL(const KURL& base, const String& relative, const WTF::TextEncoding&);

  // For conversions from other structures that have already parsed and
  // canonicalized the URL. The input must be exactly what KURL would have
  // done with the same input.
  KURL(const AtomicString& canonical_string, const url::Parsed&, bool is_valid);

  ~KURL();

  String StrippedForUseAsReferrer() const;
  String StrippedForUseAsHref() const;

  // FIXME: The above functions should be harmonized so that passing a
  // base of null or the empty string gives the same result as the
  // standard String constructor.

  // Makes a deep copy. Helpful only if you need to use a KURL on another
  // thread. Since the underlying StringImpl objects are immutable, there's
  // no other reason to ever prefer copy() over plain old assignment.
  KURL Copy() const;

  bool IsNull() const;
  bool IsEmpty() const;
  bool IsValid() const;

  // Returns true if this URL has a path. Note that "http://foo.com/" has a
  // path of "/", so this function will return true. Only invalid or
  // non-hierarchical (like "javascript:") URLs will have no path.
  bool HasPath() const;

  // Returns true if you can set the host and port for the URL.
  // Non-hierarchical URLs don't have a host and port. This is equivalent to
  // GURL::IsStandard().
  //
  // Note: this returns true for "filesystem" and false for "blob" currently,
  // due to peculiarities of how schemes are registered in url/ -- neither
  // of these schemes can have hostnames on the outer URL.
  bool CanSetHostOrPort() const { return IsHierarchical(); }
  bool CanSetPathname() const { return IsHierarchical(); }
  bool IsHierarchical() const;

  const String& GetString() const { return string_; }

  String ElidedString() const;

  String Protocol() const;
  String Host() const;

  // Returns 0 when there is no port or the default port was specified, or the
  // URL is invalid.
  //
  // We treat URLs with out-of-range port numbers as invalid URLs, and they
  // will be rejected by the canonicalizer.
  unsigned short Port() const;
  bool HasPort() const;
  String User() const;
  String Pass() const;
  String GetPath() const;
  // This method handles "parameters" separated by a semicolon.
  String LastPathComponent() const;
  String Query() const;
  String FragmentIdentifier() const;
  bool HasFragmentIdentifier() const;

  String BaseAsString() const;

  // Returns true if the current URL's protocol is the same as the StringView
  // argument. The argument must be lower-case.
  bool ProtocolIs(const StringView protocol) const;
  bool ProtocolIsData() const { return ProtocolIs("data"); }
  // This includes at least about:blank and about:srcdoc.
  bool ProtocolIsAbout() const { return ProtocolIs("about"); }
  bool ProtocolIsJavaScript() const;
  bool ProtocolIsInHTTPFamily() const;
  bool IsLocalFile() const;
  bool IsAboutBlankURL() const;   // Is exactly about:blank.
  bool IsAboutSrcdocURL() const;  // Is exactly about:srcdoc.

  bool SetProtocol(const String&);
  void SetHost(const String&);

  void RemovePort();
  void SetPort(unsigned short);
  void SetPort(const String&);

  // Input is like "foo.com" or "foo.com:8000".
  void SetHostAndPort(const String&);

  void SetUser(const String&);
  void SetPass(const String&);

  // If you pass an empty path for HTTP or HTTPS URLs, the resulting path
  // will be "/".
  void SetPath(const String&);

  // The query may begin with a question mark, or, if not, one will be added
  // for you. Setting the query to the empty string will leave a "?" in the
  // URL (with nothing after it). To clear the query, pass a null string.
  void SetQuery(const String&);

  void SetFragmentIdentifier(const String&);
  void RemoveFragmentIdentifier();

  PLATFORM_EXPORT friend bool EqualIgnoringFragmentIdentifier(const KURL&,
                                                              const KURL&);

  unsigned HostStart() const;
  unsigned HostEnd() const;

  unsigned PathStart() const;
  unsigned PathEnd() const;
  unsigned PathAfterLastSlash() const;

  operator const String&() const { return GetString(); }
  operator StringView() const { return StringView(GetString()); }

  const url::Parsed& GetParsed() const { return parsed_; }

  const KURL* InnerURL() const { return inner_url_.get(); }

  bool IsSafeToSendToAnotherThread() const;

  bool PotentiallyDanglingMarkup() const {
    return parsed_.potentially_dangling_markup;
  }

  // Returns a GURL with the same properties. This can be used in platform/ and
  // web/. However, in core/ and modules/, this should only be used to pass
  // a GURL to a layer that is expecting one instead of a KURL or a WebURL.
  // TODO(crbug.com/862940): Make this conversion explicit.
  operator GURL() const;

 private:
  friend struct WTF::HashTraits<blink::KURL>;

  void Init(const KURL& base,
            const String& relative,
            const WTF::TextEncoding* query_encoding);

  StringView ComponentStringView(const url::Component&) const;
  String ComponentString(const url::Component&) const;
  StringView StringViewForInvalidComponent() const;

  template <typename CHAR>
  void ReplaceComponents(const url::Replacements<CHAR>&);

  void InitInnerURL();
  void InitProtocolMetadata();

  bool is_valid_;
  bool protocol_is_in_http_family_;

  // Keep a separate string for the protocol to avoid copious copies for
  // protocol(). Normally this will be Atomic, except when constructed via
  // KURL::copy(), which is deep.
  String protocol_;

  url::Parsed parsed_;
  String string_;
  std::unique_ptr<KURL> inner_url_;
};

PLATFORM_EXPORT bool operator==(const KURL&, const KURL&);
PLATFORM_EXPORT bool operator==(const KURL&, const String&);
PLATFORM_EXPORT bool operator==(const String&, const KURL&);
PLATFORM_EXPORT bool operator!=(const KURL&, const KURL&);
PLATFORM_EXPORT bool operator!=(const KURL&, const String&);
PLATFORM_EXPORT bool operator!=(const String&, const KURL&);

PLATFORM_EXPORT bool EqualIgnoringFragmentIdentifier(const KURL&, const KURL&);

PLATFORM_EXPORT const KURL& BlankURL();
PLATFORM_EXPORT const KURL& SrcdocURL();
PLATFORM_EXPORT const KURL& NullURL();

// Functions to do URL operations on strings.
// These are operations that aren't faster on a parsed URL.
// These are also different from the KURL functions in that they don't require
// the string to be a valid and parsable URL.  This is especially important
// because valid javascript URLs are not necessarily considered valid by KURL.

PLATFORM_EXPORT bool ProtocolIs(const String& url, const char* protocol);
PLATFORM_EXPORT bool ProtocolIsJavaScript(const String& url);

PLATFORM_EXPORT bool IsValidProtocol(const String&);

using DecodeURLResult = url::DecodeURLResult;

// Unescapes the given string using URL escaping rules.
//
// DANGER: If the URL has "%00" in it, the resulting string will have embedded
// null characters!
//
// This function is also used to decode javascript: URLs and as a general
// purpose unescaping function.
// TODO(tkent): Remove the second argument after collecting data.
PLATFORM_EXPORT String DecodeURLEscapeSequences(const String&,
                                                DecodeURLResult* = nullptr);

PLATFORM_EXPORT String EncodeWithURLEscapeSequences(const String&);

// Inlines.

inline bool operator==(const KURL& a, const KURL& b) {
  return a.GetString() == b.GetString();
}

inline bool operator==(const KURL& a, const String& b) {
  return a.GetString() == b;
}

inline bool operator==(const String& a, const KURL& b) {
  return a == b.GetString();
}

inline bool operator!=(const KURL& a, const KURL& b) {
  return a.GetString() != b.GetString();
}

inline bool operator!=(const KURL& a, const String& b) {
  return a.GetString() != b;
}

inline bool operator!=(const String& a, const KURL& b) {
  return a != b.GetString();
}

}  // namespace blink

namespace WTF {

// KURLHash is the default hash for String
template <>
struct DefaultHash<blink::KURL> {
  typedef blink::KURLHash Hash;
};

}  // namespace WTF

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_WEBORIGIN_KURL_H_
