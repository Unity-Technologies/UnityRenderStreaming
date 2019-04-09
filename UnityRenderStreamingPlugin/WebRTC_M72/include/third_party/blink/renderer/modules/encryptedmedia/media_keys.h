/*
 * Copyright (C) 2013 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_ENCRYPTEDMEDIA_MEDIA_KEYS_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_ENCRYPTEDMEDIA_MEDIA_KEYS_H_

#include <memory>
#include "third_party/blink/public/platform/web_content_decryption_module.h"
#include "third_party/blink/public/platform/web_encrypted_media_types.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/platform/web_vector.h"
#include "third_party/blink/renderer/bindings/core/v8/active_script_wrappable.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise.h"
#include "third_party/blink/renderer/core/dom/context_lifecycle_observer.h"
#include "third_party/blink/renderer/core/typed_arrays/dom_array_piece.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/timer.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class ExceptionState;
class ExecutionContext;
class HTMLMediaElement;
class MediaKeysPolicy;
class MediaKeySession;
class ScriptState;
class WebContentDecryptionModule;

// References are held by JS and HTMLMediaElement.
// The WebContentDecryptionModule has the same lifetime as this object.
class MediaKeys : public ScriptWrappable,
                  public ActiveScriptWrappable<MediaKeys>,
                  public ContextLifecycleObserver {
  USING_GARBAGE_COLLECTED_MIXIN(MediaKeys);
  DEFINE_WRAPPERTYPEINFO();

 public:
  static MediaKeys* Create(
      ExecutionContext*,
      const WebVector<WebEncryptedMediaSessionType>& supported_session_types,
      std::unique_ptr<WebContentDecryptionModule>);

  MediaKeys(
      ExecutionContext*,
      const WebVector<WebEncryptedMediaSessionType>& supported_session_types,
      std::unique_ptr<WebContentDecryptionModule>);
  ~MediaKeys() override;

  MediaKeySession* createSession(ScriptState*,
                                 const String& session_type_string,
                                 ExceptionState&);

  ScriptPromise setServerCertificate(ScriptState*,
                                     const DOMArrayPiece& server_certificate);

  ScriptPromise getStatusForPolicy(ScriptState*, const MediaKeysPolicy*);

  // Indicates that the provided HTMLMediaElement wants to use this object.
  // Returns true if no other HTMLMediaElement currently references this
  // object, false otherwise. If true, will take a weak reference to
  // HTMLMediaElement and expects the reservation to be accepted/cancelled
  // later.
  bool ReserveForMediaElement(HTMLMediaElement*);
  // Indicates that SetMediaKeys completed successfully.
  void AcceptReservation();
  // Indicates that SetMediaKeys failed, so HTMLMediaElement did not
  // successfully link to this object.
  void CancelReservation();

  // The previously reserved and accepted HTMLMediaElement is no longer
  // using this object.
  void ClearMediaElement();

  WebContentDecryptionModule* ContentDecryptionModule();

  void Trace(blink::Visitor*) override;

  // ContextLifecycleObserver implementation.
  // FIXME: This class could derive from ContextLifecycleObserver
  // again (http://crbug.com/483722).
  void ContextDestroyed(ExecutionContext*) override;

  // ScriptWrappable implementation.
  bool HasPendingActivity() const final;

 private:
  class PendingAction;

  void SetServerCertificateTask(DOMArrayBuffer* server_certificate,
                                ContentDecryptionModuleResult*);
  void GetStatusForPolicyTask(const String& min_hdcp_version,
                              ContentDecryptionModuleResult*);

  bool SessionTypeSupported(WebEncryptedMediaSessionType);
  void TimerFired(TimerBase*);

  const WebVector<WebEncryptedMediaSessionType> supported_session_types_;
  std::unique_ptr<WebContentDecryptionModule> cdm_;

  // Keep track of the HTMLMediaElement that references this object. Keeping
  // a WeakMember so that HTMLMediaElement's lifetime isn't dependent on
  // this object.
  // Note that the referenced HTMLMediaElement must be destroyed
  // before this object. This is due to WebMediaPlayerImpl (owned by
  // HTMLMediaElement) possibly having a pointer to Decryptor created
  // by WebContentDecryptionModuleImpl (owned by this object).
  WeakMember<HTMLMediaElement> media_element_;

  // Keep track of whether this object has been reserved by HTMLMediaElement
  // (i.e. a setMediaKeys operation is in progress). Destruction of this
  // object will be prevented until the setMediaKeys() completes.
  bool reserved_for_media_element_;

  HeapDeque<Member<PendingAction>> pending_actions_;
  TaskRunnerTimer<MediaKeys> timer_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_ENCRYPTEDMEDIA_MEDIA_KEYS_H_
