// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_WORKER_FETCH_TEST_HELPER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_WORKER_FETCH_TEST_HELPER_H_

#include "base/optional.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/loader/modulescript/module_script_creation_params.h"
#include "third_party/blink/renderer/core/loader/modulescript/module_script_fetcher.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource_fetcher.h"
#include "third_party/blink/renderer/platform/loader/testing/fetch_testing_platform_support.h"
#include "third_party/blink/renderer/platform/testing/testing_platform_support.h"
#include "third_party/blink/renderer/platform/testing/unit_test_helpers.h"

namespace blink {

class ClientImpl final : public GarbageCollectedFinalized<ClientImpl>,
                         public ModuleScriptFetcher::Client {
  USING_GARBAGE_COLLECTED_MIXIN(ClientImpl);

 public:
  enum class Result { kInitial, kOK, kFailed };

  void NotifyFetchFinished(
      const base::Optional<ModuleScriptCreationParams>& params,
      const HeapVector<Member<ConsoleMessage>>&) override {
    ASSERT_EQ(Result::kInitial, result_);
    if (params) {
      result_ = Result::kOK;
      params_.emplace(*params);
    } else {
      result_ = Result::kFailed;
    }
  }

  Result GetResult() const { return result_; }
  base::Optional<ModuleScriptCreationParams> GetParams() const {
    return params_;
  }

 private:
  Result result_ = Result::kInitial;
  base::Optional<ModuleScriptCreationParams> params_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_WORKERS_WORKER_FETCH_TEST_HELPER_H_
