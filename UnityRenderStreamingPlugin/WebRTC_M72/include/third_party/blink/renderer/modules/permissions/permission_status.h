// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_PERMISSIONS_PERMISSION_STATUS_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_PERMISSIONS_PERMISSION_STATUS_H_

#include "mojo/public/cpp/bindings/binding.h"
#include "third_party/blink/public/platform/modules/permissions/permission.mojom-blink.h"
#include "third_party/blink/renderer/bindings/core/v8/active_script_wrappable.h"
#include "third_party/blink/renderer/core/dom/events/event_target.h"
#include "third_party/blink/renderer/core/dom/pausable_object.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/text/atomic_string.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class ExecutionContext;
class ScriptPromiseResolver;

// Expose the status of a given permission type for the current
// ExecutionContext.
class PermissionStatus final : public EventTargetWithInlineData,
                               public ActiveScriptWrappable<PermissionStatus>,
                               public PausableObject,
                               public mojom::blink::PermissionObserver {
  USING_GARBAGE_COLLECTED_MIXIN(PermissionStatus);
  DEFINE_WRAPPERTYPEINFO();
  USING_PRE_FINALIZER(PermissionStatus, Dispose);

  using MojoPermissionDescriptor = mojom::blink::PermissionDescriptorPtr;
  using MojoPermissionStatus = mojom::blink::PermissionStatus;

 public:
  static PermissionStatus* Take(ScriptPromiseResolver*,
                                MojoPermissionStatus,
                                MojoPermissionDescriptor);

  static PermissionStatus* CreateAndListen(ExecutionContext*,
                                           MojoPermissionStatus,
                                           MojoPermissionDescriptor);
  ~PermissionStatus() override;
  void Dispose();

  // EventTarget implementation.
  const AtomicString& InterfaceName() const override;
  ExecutionContext* GetExecutionContext() const override;

  // ScriptWrappable implementation.
  bool HasPendingActivity() const final;

  // PausableObject implementation.
  void Pause() override;
  void Unpause() override;
  void ContextDestroyed(ExecutionContext*) override;

  String state() const;

  DEFINE_ATTRIBUTE_EVENT_LISTENER(change, kChange);

  void Trace(blink::Visitor*) override;

 private:
  PermissionStatus(ExecutionContext*,
                   MojoPermissionStatus,
                   MojoPermissionDescriptor);

  void StartListening();
  void StopListening();

  void OnPermissionStatusChange(MojoPermissionStatus) override;

  MojoPermissionStatus status_;
  MojoPermissionDescriptor descriptor_;
  mojo::Binding<mojom::blink::PermissionObserver> binding_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_PERMISSIONS_PERMISSION_STATUS_H_
