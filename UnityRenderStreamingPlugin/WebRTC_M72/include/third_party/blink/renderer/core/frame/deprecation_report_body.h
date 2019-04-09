// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_DEPRECATION_REPORT_BODY_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_DEPRECATION_REPORT_BODY_H_

#include "third_party/blink/renderer/bindings/core/v8/source_location.h"
#include "third_party/blink/renderer/core/frame/message_report_body.h"

namespace blink {

class CORE_EXPORT DeprecationReportBody : public MessageReportBody {
  DEFINE_WRAPPERTYPEINFO();

 public:
  DeprecationReportBody(const String& id,
                        double anticipatedRemoval,
                        const String& message,
                        std::unique_ptr<SourceLocation> location)
      : MessageReportBody(message, std::move(location)),
        id_(id),
        anticipatedRemoval_(anticipatedRemoval) {}

  ~DeprecationReportBody() override = default;

  String id() const { return id_; }
  double anticipatedRemoval(bool& is_null) const {
    is_null = !anticipatedRemoval_;
    return anticipatedRemoval_;
  }

 private:
  const String id_;
  const double anticipatedRemoval_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_DEPRECATION_REPORT_BODY_H_
