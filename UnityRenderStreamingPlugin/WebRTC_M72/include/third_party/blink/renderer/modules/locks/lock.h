// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_LOCKS_LOCK_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_LOCKS_LOCK_H_

#include "third_party/blink/public/platform/modules/locks/lock_manager.mojom-blink.h"
#include "third_party/blink/renderer/core/dom/pausable_object.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class LockManager;
class ScriptPromise;
class ScriptPromiseResolver;
class ScriptState;

class Lock final : public ScriptWrappable, public PausableObject {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(Lock);

 public:
  static Lock* Create(ScriptState*,
                      const String& name,
                      mojom::blink::LockMode,
                      mojom::blink::LockHandlePtr,
                      LockManager*);

  Lock(ScriptState*,
       const String& name,
       mojom::blink::LockMode,
       mojom::blink::LockHandlePtr,
       LockManager*);
  ~Lock() override;

  void Trace(blink::Visitor*) override;
  EAGERLY_FINALIZE();

  // Lock.idl
  String name() const { return name_; }
  String mode() const;

  // PausableObject
  void ContextDestroyed(ExecutionContext*) override;

  // The lock is held until the passed promise resolves. When it is released,
  // the passed resolver is invoked with the promise's result.
  void HoldUntil(ScriptPromise, ScriptPromiseResolver*);

  static mojom::blink::LockMode StringToMode(const String&);
  static String ModeToString(mojom::blink::LockMode);

 private:
  class ThenFunction;

  void ReleaseIfHeld();

  void OnConnectionError();

  Member<ScriptPromiseResolver> resolver_;

  const String name_;
  const mojom::blink::LockMode mode_;

  // An opaque handle; this one end of a mojo pipe. When this is closed,
  // the lock is released by the back end.
  mojom::blink::LockHandlePtr handle_;

  // LockManager::OnLockReleased() is called when this lock is released, to
  // stop artificially keeping this instance alive. It is necessary in the
  // case where the resolver's promise could potentially be GC'd.
  Member<LockManager> manager_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_LOCKS_LOCK_H_
