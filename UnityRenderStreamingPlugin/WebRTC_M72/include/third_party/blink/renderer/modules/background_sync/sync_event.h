// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_BACKGROUND_SYNC_SYNC_EVENT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_BACKGROUND_SYNC_SYNC_EVENT_H_

#include "third_party/blink/renderer/modules/background_sync/sync_event_init.h"
#include "third_party/blink/renderer/modules/event_modules.h"
#include "third_party/blink/renderer/modules/service_worker/extendable_event.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/text/atomic_string.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class MODULES_EXPORT SyncEvent final : public ExtendableEvent {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static SyncEvent* Create(const AtomicString& type,
                           const String& tag,
                           bool last_chance,
                           WaitUntilObserver* observer) {
    return new SyncEvent(type, tag, last_chance, observer);
  }
  static SyncEvent* Create(const AtomicString& type,
                           const SyncEventInit* init) {
    return new SyncEvent(type, init);
  }

  ~SyncEvent() override;

  const AtomicString& InterfaceName() const override;

  String tag();
  bool lastChance();

  void Trace(blink::Visitor*) override;

 private:
  SyncEvent(const AtomicString& type, const String&, bool, WaitUntilObserver*);
  SyncEvent(const AtomicString& type, const SyncEventInit*);

  String tag_;
  bool last_chance_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_BACKGROUND_SYNC_SYNC_EVENT_H_
