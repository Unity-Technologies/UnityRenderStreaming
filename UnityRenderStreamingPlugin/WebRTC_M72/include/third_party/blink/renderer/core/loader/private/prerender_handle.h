/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_PRIVATE_PRERENDER_HANDLE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_PRIVATE_PRERENDER_HANDLE_H_

#include "base/macros.h"
#include "base/memory/scoped_refptr.h"
#include "third_party/blink/renderer/core/dom/context_lifecycle_observer.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"

namespace blink {

class Document;
class Prerender;
class PrerenderClient;

class PrerenderHandle final : public GarbageCollectedFinalized<PrerenderHandle>,
                              public ContextLifecycleObserver {
  USING_GARBAGE_COLLECTED_MIXIN(PrerenderHandle);

 public:
  static PrerenderHandle* Create(Document&,
                                 PrerenderClient*,
                                 const KURL&,
                                 unsigned prerender_rel_types);

  virtual ~PrerenderHandle();

  void Cancel();
  const KURL& Url() const;

  // ContextLifecycleObserver:
  void ContextDestroyed(ExecutionContext*) override;

  void Trace(blink::Visitor*) override;
  EAGERLY_FINALIZE();

 private:
  PrerenderHandle(Document&, Prerender*);

  void Detach();

  Member<Prerender> prerender_;

  DISALLOW_COPY_AND_ASSIGN(PrerenderHandle);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_PRIVATE_PRERENDER_HANDLE_H_
