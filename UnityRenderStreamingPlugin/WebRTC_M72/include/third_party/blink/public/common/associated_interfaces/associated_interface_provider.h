// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_COMMON_ASSOCIATED_INTERFACES_ASSOCIATED_INTERFACE_PROVIDER_H_
#define THIRD_PARTY_BLINK_PUBLIC_COMMON_ASSOCIATED_INTERFACES_ASSOCIATED_INTERFACE_PROVIDER_H_

#include <string>

#include "base/callback.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/single_thread_task_runner.h"
#include "mojo/public/cpp/bindings/associated_interface_ptr.h"
#include "mojo/public/cpp/bindings/associated_interface_request.h"
#include "mojo/public/cpp/bindings/scoped_interface_endpoint_handle.h"
#include "third_party/blink/public/common/common_export.h"
#include "third_party/blink/public/mojom/associated_interfaces/associated_interfaces.mojom.h"

namespace blink {

// A helper interface for connecting to remote Channel-associated interfaces.
//
// This is analogous to service_manager::InterfaceProvider in that it provides a
// means of
// binding proxies to remote interfaces, but this is specifically for interfaces
// which must be associated with an IPC::Channel, i.e. retain FIFO message
// ordering with respect to legacy IPC messages.
//
// The Channel with which the remote interfaces are associated depends on
// the configuration of the specific AssociatedInterfaceProvider instance. For
// example, RenderFrameHost exposes an instance of this class for which all
// interfaces are associated with the IPC::ChannelProxy to the render process
// which hosts its corresponding RenderFrame.
class BLINK_COMMON_EXPORT AssociatedInterfaceProvider {
 public:
  // Binds this to a remote mojom::AssociatedInterfaceProvider.
  //
  // |task_runner| must belong to the same thread. It will be used to dispatch
  // all callbacks and connection error notification.
  explicit AssociatedInterfaceProvider(
      mojom::AssociatedInterfaceProviderAssociatedPtr proxy,
      scoped_refptr<base::SingleThreadTaskRunner> task_runner = nullptr);

  // Constructs a local provider with no remote interfaces. This is useful in
  // conjunction with OverrideBinderForTesting(), in test environments where
  // there may not be a remote |mojom::AssociatedInterfaceProvider| available.
  //
  // |task_runner| must belong to the same thread. It will be used to dispatch
  // all callbacks and connection error notification.
  explicit AssociatedInterfaceProvider(
      scoped_refptr<base::SingleThreadTaskRunner> task_runner);

  ~AssociatedInterfaceProvider();

  // Passes an associated endpoint handle to the remote end to be bound to a
  // Channel-associated interface named |name|.
  void GetInterface(const std::string& name,
                    mojo::ScopedInterfaceEndpointHandle handle);

  // Templated helpers for GetInterface().
  template <typename Interface>
  void GetInterface(mojo::AssociatedInterfaceRequest<Interface> request) {
    GetInterface(Interface::Name_, request.PassHandle());
  }

  template <typename Interface>
  void GetInterface(mojo::AssociatedInterfacePtr<Interface>* proxy) {
    GetInterface(mojo::MakeRequest(proxy));
  }

  void OverrideBinderForTesting(
      const std::string& name,
      const base::RepeatingCallback<void(mojo::ScopedInterfaceEndpointHandle)>&
          binder);

 private:
  class LocalProvider;

  mojom::AssociatedInterfaceProviderAssociatedPtr proxy_;

  std::unique_ptr<LocalProvider> local_provider_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;

  DISALLOW_COPY_AND_ASSIGN(AssociatedInterfaceProvider);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_COMMON_ASSOCIATED_INTERFACES_ASSOCIATED_INTERFACE_PROVIDER_H_
