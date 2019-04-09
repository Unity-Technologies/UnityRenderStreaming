// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_FEATURE_POLICY_VIOLATION_REPORT_BODY_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_FEATURE_POLICY_VIOLATION_REPORT_BODY_H_

#include "third_party/blink/renderer/bindings/core/v8/source_location.h"
#include "third_party/blink/renderer/core/frame/message_report_body.h"

namespace blink {

class CORE_EXPORT FeaturePolicyViolationReportBody : public MessageReportBody {
  DEFINE_WRAPPERTYPEINFO();

 public:
  FeaturePolicyViolationReportBody(const String& feature_id,
                                   const String& message,
                                   const String& disposition,
                                   std::unique_ptr<SourceLocation> location)
      : MessageReportBody(message, std::move(location)),
        feature_id_(feature_id),
        disposition_(disposition) {}

  String featureId() const { return feature_id_; }
  String disposition() const { return disposition_; }

  ~FeaturePolicyViolationReportBody() override = default;

 private:
  const String feature_id_;
  const String disposition_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_FEATURE_POLICY_VIOLATION_REPORT_BODY_H_
