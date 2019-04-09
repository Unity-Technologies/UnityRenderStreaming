// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FETCH_REQUEST_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FETCH_REQUEST_H_

#include "services/network/public/mojom/fetch_api.mojom-shared.h"
#include "third_party/blink/public/platform/modules/fetch/fetch_api_request.mojom-blink.h"
#include "third_party/blink/public/platform/web_url_request.h"
#include "third_party/blink/renderer/bindings/core/v8/dictionary.h"
#include "third_party/blink/renderer/bindings/core/v8/request_or_usv_string.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/fetch/body.h"
#include "third_party/blink/renderer/core/fetch/fetch_request_data.h"
#include "third_party/blink/renderer/core/fetch/headers.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class AbortSignal;
class BodyStreamBuffer;
class ExceptionState;
class RequestInit;
class WebServiceWorkerRequest;

using RequestInfo = RequestOrUSVString;

class CORE_EXPORT Request final : public Body {
  DEFINE_WRAPPERTYPEINFO();

 public:
  // These "create" function must be called with entering an appropriate
  // V8 context.
  // From Request.idl:
  static Request* Create(ScriptState*,
                         const RequestInfo&,
                         const RequestInit*,
                         ExceptionState&);

  static Request* Create(ScriptState*, const String&, ExceptionState&);
  static Request* Create(ScriptState*,
                         const String&,
                         const RequestInit*,
                         ExceptionState&);
  static Request* Create(ScriptState*, Request*, ExceptionState&);
  static Request* Create(ScriptState*,
                         Request*,
                         const RequestInit*,
                         ExceptionState&);
  static Request* Create(ScriptState*, FetchRequestData*);
  static Request* Create(ScriptState*, const WebServiceWorkerRequest&);
  static Request* Create(ScriptState*, const mojom::blink::FetchAPIRequest&);

  // Returns false if |credentials_mode| doesn't represent a valid credentials
  // mode.
  static bool ParseCredentialsMode(const String& credentials_mode,
                                   network::mojom::FetchCredentialsMode*);

  // From Request.idl:
  String method() const;
  KURL url() const;
  Headers* getHeaders() const { return headers_; }
  String destination() const;
  String referrer() const;
  String getReferrerPolicy() const;
  String mode() const;
  String credentials() const;
  String cache() const;
  String redirect() const;
  String integrity() const;
  bool keepalive() const;
  bool isHistoryNavigation() const;
  AbortSignal* signal() const { return signal_; }

  // From Request.idl:
  // This function must be called with entering an appropriate V8 context.
  Request* clone(ScriptState*, ExceptionState&);

  FetchRequestData* PassRequestData(ScriptState*, ExceptionState&);
  mojom::blink::FetchAPIRequestPtr CreateFetchAPIRequest() const;
  bool HasBody() const;
  BodyStreamBuffer* BodyBuffer() override { return request_->Buffer(); }
  const BodyStreamBuffer* BodyBuffer() const override {
    return request_->Buffer();
  }

  void Trace(blink::Visitor*) override;

 private:
  Request(ScriptState*, FetchRequestData*, Headers*, AbortSignal*);
  Request(ScriptState*, FetchRequestData*);

  const FetchRequestData* GetRequest() const { return request_; }
  static Request* CreateRequestWithRequestOrString(ScriptState*,
                                                   Request*,
                                                   const String&,
                                                   const RequestInit*,
                                                   ExceptionState&);

  String ContentType() const override;
  String MimeType() const override;

  const TraceWrapperMember<FetchRequestData> request_;
  const Member<Headers> headers_;
  const Member<AbortSignal> signal_;
  DISALLOW_COPY_AND_ASSIGN(Request);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FETCH_REQUEST_H_
