// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_SCOPED_WEB_CALLBACKS_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_SCOPED_WEB_CALLBACKS_H_

#include <memory>
#include <utility>

#include "base/callback.h"
#include "third_party/blink/public/platform/web_callbacks.h"

namespace blink {

// A ScopedWebCallbacks is a move-only scoper which helps manage the lifetime of
// a blink::WebCallbacks object. This is particularly useful when you're
// simultaneously dealing with the following two conditions:
//
//   1. Your WebCallbacks implementation requires either onSuccess or onError to
//      be called before it's destroyed. This is the case with
//      CallbackPromiseAdapter for example, because its underlying
//      ScriptPromiseResolver must be resolved or rejected before destruction.
//
//   2. You are passing ownership of the WebCallbacks to code which may
//      silenty drop it. A common way for this to happen is to bind the
//      WebCallbacks as an argument to a base::Callback which gets destroyed
//      before it can run.
//
// While it's possible to individually track the lifetime of pending
// WebCallbacks, this becomes cumbersome when dealing with many different
// callbacks types. ScopedWebCallbacks provides a generic and relatively
// lightweight solution to this problem.
//
// Example usage:
//
//   using FooCallbacks = blink::WebCallbacks<const Foo&, const FooError&>;
//
//   void RespondWithSuccess(ScopedWebCallbacks<FooCallbacks> callbacks) {
//     callbacks.PassCallbacks()->onSuccess(Foo("everything is great"));
//   }
//
//   void OnCallbacksDropped(std::unique_ptr<FooCallbacks> callbacks) {
//     // Ownership of the FooCallbacks is passed to this function if
//     // ScopedWebCallbacks::PassCallbacks isn't called before the
//     // ScopedWebCallbacks is destroyed.
//     callbacks->onError(FooError("everything is terrible"));
//   }
//
//   // Blink client implementation
//   void FooClientImpl::doMagic(std::unique_ptr<FooCallbacks> callbacks) {
//     auto scoped_callbacks = make_scoped_web_callbacks(
//         std::move(callbacks), base::BindOnce(&OnCallbacksDropped));
//
//     // Call to some lower-level service which may never run the callback we
//     // give it.
//     foo_service_->DoMagic(base::BindOnce(&RespondWithSuccess,
//                                          std::move(scoped_callbacks)));
//   }
//
// If the bound RespondWithSuccess callback actually runs, PassCallbacks() will
// reliquish ownership of the WebCallbacks object to a temporary scoped_ptr
// which will be destroyed immediately after onSuccess is called.
//
// If the bound RespondWithSuccess callback is instead destroyed first,
// the ScopedWebCallbacks destructor will invoke OnCallbacksDropped, executing
// our desired default behavior before deleting the WebCallbacks.
template <typename CallbacksType>
class ScopedWebCallbacks {
 public:
  using DestructionCallback =
      base::OnceCallback<void(std::unique_ptr<CallbacksType> callbacks)>;

  ScopedWebCallbacks(std::unique_ptr<CallbacksType> callbacks,
                     DestructionCallback destruction_callback)
      : callbacks_(std::move(callbacks)),
        destruction_callback_(std::move(destruction_callback)) {}

  ~ScopedWebCallbacks() {
    if (destruction_callback_)
      std::move(destruction_callback_).Run(std::move(callbacks_));
  }

  ScopedWebCallbacks(ScopedWebCallbacks&& other) = default;
  ScopedWebCallbacks(const ScopedWebCallbacks& other) = delete;

  ScopedWebCallbacks& operator=(ScopedWebCallbacks&& other) = default;
  ScopedWebCallbacks& operator=(const ScopedWebCallbacks& other) = delete;

  std::unique_ptr<CallbacksType> PassCallbacks() {
    destruction_callback_ = DestructionCallback();
    return std::move(callbacks_);
  }

 private:
  std::unique_ptr<CallbacksType> callbacks_;
  DestructionCallback destruction_callback_;
};

template <typename CallbacksType>
ScopedWebCallbacks<CallbacksType> MakeScopedWebCallbacks(
    std::unique_ptr<CallbacksType> callbacks,
    typename ScopedWebCallbacks<CallbacksType>::DestructionCallback
        destruction_callback) {
  return ScopedWebCallbacks<CallbacksType>(std::move(callbacks),
                                           std::move(destruction_callback));
}

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_SCOPED_WEB_CALLBACKS_H_
