// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_INSPECTOR_INSPECTOR_TESTING_AGENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_INSPECTOR_INSPECTOR_TESTING_AGENT_H_

#include "base/macros.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/inspector/inspector_base_agent.h"
#include "third_party/blink/renderer/core/inspector/protocol/Testing.h"

namespace blink {

class InspectedFrames;

class CORE_EXPORT InspectorTestingAgent final
    : public InspectorBaseAgent<protocol::Testing::Metainfo> {
 public:
  InspectorTestingAgent(InspectedFrames*);
  ~InspectorTestingAgent() override;
  void Trace(blink::Visitor*) override;

  protocol::Response generateTestReport(const String& message,
                                        protocol::Maybe<String> group) override;

 private:
  Member<InspectedFrames> inspected_frames_;
  DISALLOW_COPY_AND_ASSIGN(InspectorTestingAgent);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_INSPECTOR_INSPECTOR_TESTING_AGENT_H_
