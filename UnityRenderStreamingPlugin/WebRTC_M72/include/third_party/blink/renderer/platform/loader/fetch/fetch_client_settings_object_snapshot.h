// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_LOADER_FETCH_FETCH_CLIENT_SETTINGS_OBJECT_SNAPSHOT_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_LOADER_FETCH_FETCH_CLIENT_SETTINGS_OBJECT_SNAPSHOT_H_

#include "third_party/blink/renderer/platform/cross_thread_copier.h"
#include "third_party/blink/renderer/platform/loader/fetch/fetch_client_settings_object.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/weborigin/referrer_policy.h"
#include "third_party/blink/renderer/platform/weborigin/security_origin.h"

namespace blink {

// This class is needed to copy a FetchClientSettingsObjectSnapshot across
// threads, because it has some members which cannot be transferred across
// threads (AtomicString for example).
// There are some rules / restrictions:
//   - This struct cannot contain an object that cannot be transferred across
//     threads (e.g., AtomicString)
//   - Non-simple members need explicit copying (e.g., String::IsolatedCopy,
//     KURL::Copy) rather than the copy constructor or the assignment operator.
struct CrossThreadFetchClientSettingsObjectData {
  WTF_MAKE_NONCOPYABLE(CrossThreadFetchClientSettingsObjectData);
  USING_FAST_MALLOC(CrossThreadFetchClientSettingsObjectData);

 public:
  CrossThreadFetchClientSettingsObjectData(
      KURL base_url,
      scoped_refptr<const SecurityOrigin> security_origin,
      ReferrerPolicy referrer_policy,
      String outgoing_referrer,
      HttpsState https_state)
      : base_url(std::move(base_url)),
        security_origin(std::move(security_origin)),
        referrer_policy(referrer_policy),
        outgoing_referrer(std::move(outgoing_referrer)),
        https_state(https_state) {}

  const KURL base_url;
  const scoped_refptr<const SecurityOrigin> security_origin;
  const ReferrerPolicy referrer_policy;
  const String outgoing_referrer;
  const HttpsState https_state;
};

// This takes a partial snapshot of the execution context's states so that an
// instance of this class can be passed to another thread without cross-thread
// synchronization. Don't keep this object persistently, instead create a new
// instance per each "fetch a module script graph" algorithm:
// https://html.spec.whatwg.org/multipage/webappapis.html#fetch-a-module-script-tree
// https://html.spec.whatwg.org/multipage/webappapis.html#fetch-a-module-worker-script-tree
//
// This class should be used only for main worker (worklet) script loading. For
// other resources, FetchClientSettingsObjectImpl should be used. See the class
// level comments on that class.
class PLATFORM_EXPORT FetchClientSettingsObjectSnapshot final
    : public FetchClientSettingsObject {
 public:
  explicit FetchClientSettingsObjectSnapshot(
      std::unique_ptr<CrossThreadFetchClientSettingsObjectData>);
  FetchClientSettingsObjectSnapshot(
      const KURL& base_url,
      const scoped_refptr<const SecurityOrigin> security_origin,
      ReferrerPolicy referrer_policy,
      const String& outgoing_referrer,
      HttpsState https_state);

  ~FetchClientSettingsObjectSnapshot() override = default;

  const KURL& BaseURL() const override { return base_url_; }
  const SecurityOrigin* GetSecurityOrigin() const override {
    return security_origin_.get();
  }
  ReferrerPolicy GetReferrerPolicy() const override { return referrer_policy_; }
  const String GetOutgoingReferrer() const override {
    return outgoing_referrer_;
  }
  HttpsState GetHttpsState() const override { return https_state_; }

  // Gets a copy of the data suitable for passing to another thread.
  std::unique_ptr<CrossThreadFetchClientSettingsObjectData> CopyData() const {
    return std::make_unique<CrossThreadFetchClientSettingsObjectData>(
        base_url_.Copy(), security_origin_->IsolatedCopy(), referrer_policy_,
        outgoing_referrer_.IsolatedCopy(), https_state_);
  }

 private:
  const KURL base_url_;
  const scoped_refptr<const SecurityOrigin> security_origin_;
  const ReferrerPolicy referrer_policy_;
  const String outgoing_referrer_;
  const HttpsState https_state_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_LOADER_FETCH_FETCH_CLIENT_SETTINGS_OBJECT_SNAPSHOT_H_
