// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_BACKGROUND_FETCH_BACKGROUND_FETCH_UPDATE_UI_EVENT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_BACKGROUND_FETCH_BACKGROUND_FETCH_UPDATE_UI_EVENT_H_

#include "third_party/blink/public/platform/modules/background_fetch/background_fetch.mojom-blink.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise.h"
#include "third_party/blink/renderer/modules/background_fetch/background_fetch_event.h"
#include "third_party/blink/renderer/modules/service_worker/service_worker_registration.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/wtf/text/atomic_string.h"
#include "third_party/skia/include/core/SkBitmap.h"

namespace blink {

class BackgroundFetchEvent;
class BackgroundFetchIconLoader;
class BackgroundFetchUIOptions;
class ScriptPromiseResolver;
class WaitUntilObserver;

// Event for interacting with fetch requests that have completed.
class MODULES_EXPORT BackgroundFetchUpdateUIEvent final
    : public BackgroundFetchEvent {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static BackgroundFetchUpdateUIEvent* Create(
      const AtomicString& type,
      const BackgroundFetchEventInit* initializer) {
    return new BackgroundFetchUpdateUIEvent(type, initializer);
  }

  static BackgroundFetchUpdateUIEvent* Create(
      const AtomicString& type,
      const BackgroundFetchEventInit* initializer,
      WaitUntilObserver* observer,
      ServiceWorkerRegistration* registration) {
    return new BackgroundFetchUpdateUIEvent(type, initializer, observer,
                                            registration);
  }

  ~BackgroundFetchUpdateUIEvent() override;

  // Web Exposed method defined in the IDL file.
  ScriptPromise updateUI(ScriptState* script_state,
                         const BackgroundFetchUIOptions* ui_options);

  void Trace(blink::Visitor* visitor) override;

 private:
  BackgroundFetchUpdateUIEvent(const AtomicString& type,
                               const BackgroundFetchEventInit* initializer);

  BackgroundFetchUpdateUIEvent(const AtomicString& type,
                               const BackgroundFetchEventInit* init,
                               WaitUntilObserver* observer,
                               ServiceWorkerRegistration* registration);

  void DidGetIcon(ScriptPromiseResolver* resolver,
                  const String& title,
                  const SkBitmap& icon,
                  int64_t ideal_to_chosen_icon_size);

  void DidUpdateUI(ScriptPromiseResolver* resolver,
                   mojom::blink::BackgroundFetchError error);

  bool update_ui_called_ = false;

  Member<ServiceWorkerRegistration> service_worker_registration_;
  Member<BackgroundFetchIconLoader> loader_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_BACKGROUND_FETCH_BACKGROUND_FETCH_UPDATE_UI_EVENT_H_
