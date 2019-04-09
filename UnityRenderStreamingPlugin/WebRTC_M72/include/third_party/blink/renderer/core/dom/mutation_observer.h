/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_DOM_MUTATION_OBSERVER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_DOM_MUTATION_OBSERVER_H_

#include "base/gtest_prod_util.h"
#include "third_party/blink/renderer/bindings/core/v8/active_script_wrappable.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/context_lifecycle_observer.h"
#include "third_party/blink/renderer/core/dom/mutation_observer_options.h"
#include "third_party/blink/renderer/core/execution_context/execution_context.h"
#include "third_party/blink/renderer/platform/bindings/name_client.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/bindings/trace_wrapper_member.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/hash_set.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class Document;
class ExceptionState;
class HTMLSlotElement;
class MutationObserver;
class MutationObserverInit;
class MutationObserverRegistration;
class MutationRecord;
class Node;
class ScriptState;
class V8MutationCallback;

using MutationObserverSet = HeapHashSet<Member<MutationObserver>>;
using MutationObserverRegistrationSet =
    HeapHashSet<WeakMember<MutationObserverRegistration>>;
using MutationObserverVector = HeapVector<Member<MutationObserver>>;
using MutationRecordVector = HeapVector<Member<MutationRecord>>;

class CORE_EXPORT MutationObserver final
    : public ScriptWrappable,
      public ActiveScriptWrappable<MutationObserver>,
      public ContextClient {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(MutationObserver);

 public:
  enum ObservationFlags { kSubtree = 1 << 3, kAttributeFilter = 1 << 4 };

  enum DeliveryFlags {
    kAttributeOldValue = 1 << 5,
    kCharacterDataOldValue = 1 << 6,
  };

  class CORE_EXPORT Delegate : public GarbageCollectedFinalized<Delegate>,
                               public NameClient {
   public:
    virtual ~Delegate() = default;
    virtual ExecutionContext* GetExecutionContext() const = 0;
    virtual void Deliver(const MutationRecordVector& records,
                         MutationObserver&) = 0;
    virtual void Trace(blink::Visitor* visitor) {}
    const char* NameInHeapSnapshot() const override {
      return "MutationObserver::Delegate";
    }
  };

  class CORE_EXPORT V8DelegateImpl;

  static MutationObserver* Create(Delegate*);
  static MutationObserver* Create(ScriptState*, V8MutationCallback*);
  static void ResumeSuspendedObservers();
  static void DeliverMutations();
  static void EnqueueSlotChange(HTMLSlotElement&);
  static void CleanSlotChangeList(Document&);

  MutationObserver(ExecutionContext*, Delegate*);
  ~MutationObserver() override;

  void observe(Node*, const MutationObserverInit*, ExceptionState&);
  MutationRecordVector takeRecords();
  void disconnect();
  void ObservationStarted(MutationObserverRegistration*);
  void ObservationEnded(MutationObserverRegistration*);
  void EnqueueMutationRecord(MutationRecord*);
  void SetHasTransientRegistration();

  HeapHashSet<Member<Node>> GetObservedNodes() const;

  bool HasPendingActivity() const override { return !records_.IsEmpty(); }

  // Eagerly finalized as destructor accesses heap object members.
  EAGERLY_FINALIZE();
  void Trace(blink::Visitor*) override;

 private:
  struct ObserverLessThan;

  void Deliver();
  bool ShouldBeSuspended() const;
  void CancelInspectorAsyncTasks();

  TraceWrapperMember<Delegate> delegate_;
  HeapVector<TraceWrapperMember<MutationRecord>> records_;
  MutationObserverRegistrationSet registrations_;
  unsigned priority_;

  FRIEND_TEST_ALL_PREFIXES(MutationObserverTest, DisconnectCrash);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_DOM_MUTATION_OBSERVER_H_
