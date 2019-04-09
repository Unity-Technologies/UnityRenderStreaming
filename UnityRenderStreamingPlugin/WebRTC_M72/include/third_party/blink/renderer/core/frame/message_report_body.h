// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_MESSAGE_REPORT_BODY_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_MESSAGE_REPORT_BODY_H_

#include "third_party/blink/renderer/bindings/core/v8/source_location.h"
#include "third_party/blink/renderer/core/frame/report_body.h"

namespace blink {

class MessageReportBody : public ReportBody {
 public:
  MessageReportBody(const String& message,
                    std::unique_ptr<SourceLocation> location)
      : message_(message), location_(std::move(location)) {}

  ~MessageReportBody() override = default;

  String message() const { return message_; }

  String sourceFile() const { return location_->Url(); }

  uint32_t lineNumber(bool& is_null) const {
    is_null = location_->IsUnknown();
    return location_->LineNumber();
  }

  uint32_t columnNumber(bool& is_null) const {
    is_null = location_->IsUnknown();
    return location_->ColumnNumber();
  }

 protected:
  const String message_;
  std::unique_ptr<SourceLocation> location_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_MESSAGE_REPORT_BODY_H_
