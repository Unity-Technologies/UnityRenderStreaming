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

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_ENCRYPTEDMEDIA_MEDIA_KEY_SESSION_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_ENCRYPTEDMEDIA_MEDIA_KEY_SESSION_H_

#include <memory>
#include "third_party/blink/public/platform/web_content_decryption_module_session.h"
#include "third_party/blink/public/platform/web_encrypted_media_types.h"
#include "third_party/blink/renderer/bindings/core/v8/active_script_wrappable.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise_property.h"
#include "third_party/blink/renderer/core/dom/context_lifecycle_observer.h"
#include "third_party/blink/renderer/core/typed_arrays/dom_array_piece.h"
#include "third_party/blink/renderer/modules/encryptedmedia/media_key_status_map.h"
#include "third_party/blink/renderer/modules/event_target_modules.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/timer.h"

namespace blink {

class DOMException;
class EventQueue;
class MediaKeys;

// References are held by JS only. However, even if all JS references are
// dropped, it won't be garbage collected until close event received or
// MediaKeys goes away (as determined by a WeakMember reference). This allows
// the CDM to continue to fire events for this session, as long as the session
// is open.
//
// WeakMember<MediaKeys> is used instead of having MediaKeys and MediaKeySession
// keep references to each other, and then having to inform the other object
// when it gets destroyed. When the Oilpan garbage collector determines that
// only WeakMember<> references remain to the MediaKeys object, the MediaKeys
// object will be finalized and the WeakMember<> references will be cleared
// out(zeroed) by the garbage collector.
//
// Because this object controls the lifetime of the
// WebContentDecryptionModuleSession, it may outlive any JavaScript references
// as long as the MediaKeys object is alive.
// The WebContentDecryptionModuleSession has the same lifetime as this object.
class MediaKeySession final
    : public EventTargetWithInlineData,
      public ActiveScriptWrappable<MediaKeySession>,
      public ContextLifecycleObserver,
      private WebContentDecryptionModuleSession::Client {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(MediaKeySession);
  USING_PRE_FINALIZER(MediaKeySession, Dispose);

 public:
  static MediaKeySession* Create(ScriptState*,
                                 MediaKeys*,
                                 WebEncryptedMediaSessionType);

  MediaKeySession(ScriptState*, MediaKeys*, WebEncryptedMediaSessionType);
  ~MediaKeySession() override;

  String sessionId() const;
  double expiration() const { return expiration_; }
  ScriptPromise closed(ScriptState*);
  MediaKeyStatusMap* keyStatuses();
  DEFINE_ATTRIBUTE_EVENT_LISTENER(keystatuseschange, kKeystatuseschange);
  DEFINE_ATTRIBUTE_EVENT_LISTENER(message, kMessage);

  ScriptPromise generateRequest(ScriptState*,
                                const String& init_data_type,
                                const DOMArrayPiece& init_data);
  ScriptPromise load(ScriptState*, const String& session_id);
  ScriptPromise update(ScriptState*, const DOMArrayPiece& response);
  ScriptPromise close(ScriptState*);
  ScriptPromise remove(ScriptState*);

  // EventTarget
  const AtomicString& InterfaceName() const override;
  ExecutionContext* GetExecutionContext() const override;

  // ScriptWrappable
  bool HasPendingActivity() const final;

  // ContextLifecycleObserver
  void ContextDestroyed(ExecutionContext*) override;

  void Trace(blink::Visitor*) override;

 private:
  class PendingAction;
  friend class NewSessionResultPromise;
  friend class LoadSessionResultPromise;

  void Dispose();

  void ActionTimerFired(TimerBase*);

  // The following perform the asynchronous part of the command referenced.
  void GenerateRequestTask(ContentDecryptionModuleResult*,
                           WebEncryptedMediaInitDataType,
                           DOMArrayBuffer* init_data_buffer);
  void FinishGenerateRequest();
  void LoadTask(ContentDecryptionModuleResult*, const String& session_id);
  void FinishLoad();
  void UpdateTask(ContentDecryptionModuleResult*,
                  DOMArrayBuffer* sanitized_response);
  void CloseTask(ContentDecryptionModuleResult*);
  void RemoveTask(ContentDecryptionModuleResult*);

  // WebContentDecryptionModuleSession::Client
  void Message(MessageType,
               const unsigned char* message,
               size_t message_length) override;
  void Close() override;
  void ExpirationChanged(double updated_expiry_time_in_ms) override;
  void KeysStatusesChange(const WebVector<WebEncryptedMediaKeyInformation>&,
                          bool has_additional_usable_key) override;

  Member<EventQueue> async_event_queue_;
  std::unique_ptr<WebContentDecryptionModuleSession> session_;

  // Used to determine if MediaKeys is still active.
  WeakMember<MediaKeys> media_keys_;

  // Session properties.
  WebEncryptedMediaSessionType session_type_;
  double expiration_;
  Member<MediaKeyStatusMap> key_statuses_map_;

  // Session states.
  bool is_uninitialized_;
  bool is_callable_;
  bool is_closing_or_closed_;

  // Keep track of the closed promise.
  typedef ScriptPromiseProperty<Member<MediaKeySession>,
                                ToV8UndefinedGenerator,
                                Member<DOMException>>
      ClosedPromise;
  Member<ClosedPromise> closed_promise_;

  HeapDeque<Member<PendingAction>> pending_actions_;
  TaskRunnerTimer<MediaKeySession> action_timer_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_ENCRYPTEDMEDIA_MEDIA_KEY_SESSION_H_
