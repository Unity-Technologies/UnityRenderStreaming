/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY GOOGLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL GOOGLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_PEERCONNECTION_RTC_DTMF_SENDER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_PEERCONNECTION_RTC_DTMF_SENDER_H_

#include <memory>
#include "third_party/blink/public/platform/web_rtc_dtmf_sender_handler_client.h"
#include "third_party/blink/renderer/core/dom/context_lifecycle_observer.h"
#include "third_party/blink/renderer/modules/event_target_modules.h"
#include "third_party/blink/renderer/platform/timer.h"

namespace blink {

class ExceptionState;
class WebRTCDTMFSenderHandler;

class RTCDTMFSender final : public EventTargetWithInlineData,
                            public WebRTCDTMFSenderHandlerClient,
                            public ContextLifecycleObserver {
  USING_GARBAGE_COLLECTED_MIXIN(RTCDTMFSender);
  DEFINE_WRAPPERTYPEINFO();
  USING_PRE_FINALIZER(RTCDTMFSender, Dispose);

 public:
  static RTCDTMFSender* Create(ExecutionContext*,
                               std::unique_ptr<WebRTCDTMFSenderHandler>);
  ~RTCDTMFSender() override;

  bool canInsertDTMF() const;
  String toneBuffer() const;

  void insertDTMF(const String& tones, ExceptionState&);
  void insertDTMF(const String& tones, int duration, ExceptionState&);
  void insertDTMF(const String& tones,
                  int duration,
                  int inter_tone_gap,
                  ExceptionState&);

  DEFINE_ATTRIBUTE_EVENT_LISTENER(tonechange, kTonechange);

  // EventTarget
  const AtomicString& InterfaceName() const override;
  ExecutionContext* GetExecutionContext() const override;

  // ContextLifecycleObserver
  void ContextDestroyed(ExecutionContext*) override;

  void Trace(blink::Visitor*) override;

 private:
  RTCDTMFSender(ExecutionContext*,
                std::unique_ptr<WebRTCDTMFSenderHandler>);
  void Dispose();

  // WebRTCDTMFSenderHandlerClient
  void PlayoutTask();
  void DidPlayTone(const WebString&) override;

  std::unique_ptr<WebRTCDTMFSenderHandler> handler_;

  bool stopped_;
  String tone_buffer_;
  int duration_;
  int inter_tone_gap_;
  bool playout_task_is_scheduled_ = false;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_PEERCONNECTION_RTC_DTMF_SENDER_H_
