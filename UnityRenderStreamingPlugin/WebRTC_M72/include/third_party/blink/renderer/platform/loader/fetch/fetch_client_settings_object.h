// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_LOADER_FETCH_FETCH_CLIENT_SETTINGS_OBJECT_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_LOADER_FETCH_FETCH_CLIENT_SETTINGS_OBJECT_H_

#include "third_party/blink/renderer/platform/cross_thread_copier.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/heap/heap.h"
#include "third_party/blink/renderer/platform/loader/fetch/https_state.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/weborigin/referrer_policy.h"
#include "third_party/blink/renderer/platform/weborigin/security_origin.h"

namespace blink {

// This is a partial interface of the "settings object" concept defined in the
// HTML spec:
// https://html.spec.whatwg.org/multipage/webappapis.html#settings-object
//
// This is also a partial interface of the "fetch client settings object" used
// in module script fetch. Other part of the "fetch client settings object" is
// currently implemented by ResourceFetcher and FetchContext, and this class is
// used together with them.
// https://html.spec.whatwg.org/multipage/webappapis.html#fetch-a-module-worker-script-tree
class PLATFORM_EXPORT FetchClientSettingsObject
    : public GarbageCollectedFinalized<FetchClientSettingsObject> {
 public:
  virtual ~FetchClientSettingsObject() = default;

  // "A URL used by APIs called by scripts that use this environment settings
  // object to parse URLs."
  // https://html.spec.whatwg.org/multipage/webappapis.html#api-base-url
  virtual const KURL& BaseURL() const = 0;

  // "An origin used in security checks."
  // https://html.spec.whatwg.org/multipage/webappapis.html#concept-settings-object-origin
  virtual const SecurityOrigin* GetSecurityOrigin() const = 0;

  // "The default referrer policy for fetches performed using this environment
  // settings object as a request client."
  // https://html.spec.whatwg.org/multipage/webappapis.html#concept-settings-object-referrer-policy
  virtual ReferrerPolicy GetReferrerPolicy() const = 0;

  // "referrerURL" used in the "Determine request's Referrer" algorithm:
  // https://w3c.github.io/webappsec-referrer-policy/#determine-requests-referrer
  virtual const String GetOutgoingReferrer() const = 0;

  // https://html.spec.whatwg.org/multipage/webappapis.html#https-state
  virtual HttpsState GetHttpsState() const = 0;

  virtual void Trace(Visitor*) {}
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_LOADER_FETCH_FETCH_CLIENT_SETTINGS_OBJECT_H_
