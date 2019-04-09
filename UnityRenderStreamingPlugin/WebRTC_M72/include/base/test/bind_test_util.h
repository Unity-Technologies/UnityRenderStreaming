// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TEST_BIND_TEST_UTIL_H_
#define BASE_TEST_BIND_TEST_UTIL_H_

#include "base/bind.h"

namespace base {
namespace internal {

template <typename F, typename Signature>
struct BindLambdaHelper;

template <typename F, typename R, typename... Args>
struct BindLambdaHelper<F, R(Args...)> {
  static R Run(const std::decay_t<F>& f, Args... args) {
    return f(std::forward<Args>(args)...);
  }
};

}  // namespace internal

// A variant of Bind() that can bind capturing lambdas for testing.
// This doesn't support extra arguments binding as the lambda itself can do.
template <typename F>
decltype(auto) BindLambdaForTesting(F&& f) {
  using Signature = internal::ExtractCallableRunType<std::decay_t<F>>;
  return BindRepeating(&internal::BindLambdaHelper<F, Signature>::Run,
                       std::forward<F>(f));
}

}  // namespace base

#endif  // BASE_TEST_BIND_TEST_UTIL_H_
