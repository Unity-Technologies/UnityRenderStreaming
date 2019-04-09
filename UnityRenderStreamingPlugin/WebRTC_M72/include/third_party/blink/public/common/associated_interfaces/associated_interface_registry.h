// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_COMMON_ASSOCIATED_INTERFACES_ASSOCIATED_INTERFACE_REGISTRY_H_
#define THIRD_PARTY_BLINK_PUBLIC_COMMON_ASSOCIATED_INTERFACES_ASSOCIATED_INTERFACE_REGISTRY_H_

#include <map>
#include <string>

#include "base/bind.h"
#include "base/callback.h"
#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "mojo/public/cpp/bindings/associated_interface_request.h"
#include "mojo/public/cpp/bindings/scoped_interface_endpoint_handle.h"
#include "third_party/blink/public/common/common_export.h"

namespace blink {

// An AssociatedInterfaceRegistry is a collection of associated interface-
// binding callbacks mapped by interface name.
//
// This is used to register binding callbacks for interfaces which must be
// associated with some IPC::ChannelProxy, meaning that messages on the
// interface retain FIFO with respect to legacy Chrome IPC messages sent or
// dispatched on the channel.
//
// The channel with which a registered interface is associated depends on the
// configuration of the specific AssociatedInterfaceRegistry instance. For
// example, RenderFrame exposes an instance of this class for which all
// interfaces are associated with the IPC::SyncChannel to the browser.
class BLINK_COMMON_EXPORT AssociatedInterfaceRegistry {
 public:
  using Binder =
      base::RepeatingCallback<void(mojo::ScopedInterfaceEndpointHandle)>;

  AssociatedInterfaceRegistry();
  ~AssociatedInterfaceRegistry();

  // Adds an interface binder to the registry.
  void AddInterface(const std::string& name, const Binder& binder);

  // Removes an interface binder from the registry.
  void RemoveInterface(const std::string& name);

  // Attempts to bind an interface named |interface_name| using a registered
  // binder. If no matching binder exists, this returns |false| and |*handle| is
  // untouched. Otherwise this returns |true| and ownership of |*handle| is
  // taken.
  bool TryBindInterface(const std::string& name,
                        mojo::ScopedInterfaceEndpointHandle* handle);

  template <typename Interface>
  using InterfaceBinder = base::RepeatingCallback<void(
      mojo::AssociatedInterfaceRequest<Interface>)>;

  // Templated helper for AddInterface() above.
  template <typename Interface>
  void AddInterface(const InterfaceBinder<Interface>& binder) {
    AddInterface(Interface::Name_,
                 base::BindRepeating(&BindInterface<Interface>, binder));
  }

  base::WeakPtr<AssociatedInterfaceRegistry> GetWeakPtr();

 private:
  template <typename Interface>
  static void BindInterface(const InterfaceBinder<Interface>& binder,
                            mojo::ScopedInterfaceEndpointHandle handle) {
    binder.Run(mojo::AssociatedInterfaceRequest<Interface>(std::move(handle)));
  }

  std::map<std::string, Binder> interfaces_;
  base::WeakPtrFactory<AssociatedInterfaceRegistry> weak_ptr_factory_{this};

  DISALLOW_COPY_AND_ASSIGN(AssociatedInterfaceRegistry);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_COMMON_ASSOCIATED_INTERFACES_ASSOCIATED_INTERFACE_REGISTRY_H_
