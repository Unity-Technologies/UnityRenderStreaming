// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_MOJO_CANONICAL_COOKIE_MOJOM_TRAITS_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_MOJO_CANONICAL_COOKIE_MOJOM_TRAITS_H_

#include "base/time/time.h"
#include "mojo/public/cpp/bindings/struct_traits.h"
#include "services/network/public/mojom/restricted_cookie_manager.mojom-blink.h"
#include "third_party/blink/public/platform/web_canonical_cookie.h"
#include "third_party/blink/public/platform/web_string.h"

namespace mojo {

template <>
struct PLATFORM_EXPORT StructTraits<network::mojom::CanonicalCookieDataView,
                                    blink::WebCanonicalCookie> {
  static WTF::String name(const blink::WebCanonicalCookie& c);
  static WTF::String value(const blink::WebCanonicalCookie& c);
  static WTF::String domain(const blink::WebCanonicalCookie& c);
  static WTF::String path(const blink::WebCanonicalCookie& c);
  static base::Time creation(const blink::WebCanonicalCookie& c);
  static base::Time expiry(const blink::WebCanonicalCookie& c);
  static base::Time last_access(const blink::WebCanonicalCookie& c);
  static bool secure(const blink::WebCanonicalCookie& c);
  static bool httponly(const blink::WebCanonicalCookie& c);
  static network::mojom::CookieSameSite site_restrictions(
      const blink::WebCanonicalCookie& c);
  static network::mojom::CookiePriority priority(
      const blink::WebCanonicalCookie& c);

  static bool Read(network::mojom::CanonicalCookieDataView cookie,
                   blink::WebCanonicalCookie* out);
};

}  // namespace mojo

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_MOJO_CANONICAL_COOKIE_MOJOM_TRAITS_H_
