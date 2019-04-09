// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SCRIPT_FETCH_CLIENT_SETTINGS_OBJECT_IMPL_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SCRIPT_FETCH_CLIENT_SETTINGS_OBJECT_IMPL_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/cross_thread_copier.h"
#include "third_party/blink/renderer/platform/heap/member.h"
#include "third_party/blink/renderer/platform/loader/fetch/fetch_client_settings_object.h"
#include "third_party/blink/renderer/platform/loader/fetch/https_state.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/weborigin/referrer_policy.h"
#include "third_party/blink/renderer/platform/weborigin/security_origin.h"

namespace blink {

class ExecutionContext;

// This is an implementation of FetchClientSettingsObject. As opposed to
// FetchClientSettingsObjectSnapshot, this refers to up-to-date values of the
// settings object.
//
// This class should be used for resource loading other than main worker
// (worklet) scripts. For the main scripts, FetchClientSettingsObjectSnapshot
// should be used. See the class level comments on that class.
class CORE_EXPORT FetchClientSettingsObjectImpl final
    : public FetchClientSettingsObject {
 public:
  explicit FetchClientSettingsObjectImpl(ExecutionContext&);
  ~FetchClientSettingsObjectImpl() override = default;

  const KURL& BaseURL() const override;
  const SecurityOrigin* GetSecurityOrigin() const override;
  ReferrerPolicy GetReferrerPolicy() const override;
  const String GetOutgoingReferrer() const override;

  HttpsState GetHttpsState() const override;

  void Trace(Visitor* visitor) override;

 private:
  Member<ExecutionContext> execution_context_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SCRIPT_FETCH_CLIENT_SETTINGS_OBJECT_IMPL_H_
