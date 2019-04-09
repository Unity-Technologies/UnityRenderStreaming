// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FEATURE_POLICY_DOCUMENT_POLICY_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FEATURE_POLICY_DOCUMENT_POLICY_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/feature_policy/policy.h"
#include "third_party/blink/renderer/platform/heap/member.h"

namespace blink {

// DocumentPolicy inherits Policy. It represents the feature policy
// introspection of a document.
class CORE_EXPORT DocumentPolicy final : public Policy {
 public:
  // Create a new DocumentPolicy, which is associated with |document|.
  explicit DocumentPolicy(Document* document) : document_(document) {}

  void Trace(blink::Visitor* visitor) override {
    visitor->Trace(document_);
    ScriptWrappable::Trace(visitor);
  }

 protected:
  const FeaturePolicy* GetPolicy() const override {
    return document_->GetFeaturePolicy();
  }
  Document* GetDocument() const override { return document_; }

 private:
  Member<Document> document_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FEATURE_POLICY_DOCUMENT_POLICY_H_
