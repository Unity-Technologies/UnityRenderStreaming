// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_SERVER_TIMING_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_SERVER_TIMING_H_

#include "third_party/blink/public/platform/web_resource_timing_info.h"
#include "third_party/blink/public/platform/web_vector.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_object_builder.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class ResourceTimingInfo;
class PerformanceServerTiming;

class CORE_EXPORT PerformanceServerTiming final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  ~PerformanceServerTiming() override;

  const String& name() const { return name_; }
  double duration() const { return duration_; }
  const String& description() const { return description_; }

  static WebVector<WebServerTimingInfo> ParseServerTiming(
      const ResourceTimingInfo&);
  static HeapVector<Member<PerformanceServerTiming>> FromParsedServerTiming(
      const WebVector<WebServerTimingInfo>&);

  ScriptValue toJSONForBinding(ScriptState*) const;

 private:
  PerformanceServerTiming(const String& name,
                          double duration,
                          const String& description);

  const String name_;
  double duration_;
  const String description_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_TIMING_PERFORMANCE_SERVER_TIMING_H_
