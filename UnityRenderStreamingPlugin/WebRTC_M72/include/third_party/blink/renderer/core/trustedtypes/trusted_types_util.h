// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_TRUSTEDTYPES_TRUSTED_TYPES_UTIL_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_TRUSTEDTYPES_TRUSTED_TYPES_UTIL_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class Document;
class ExceptionState;
class StringOrTrustedHTML;
class StringOrTrustedHTMLOrTrustedScriptOrTrustedScriptURLOrTrustedURL;
class StringOrTrustedScript;
class StringOrTrustedScriptURL;
class USVStringOrTrustedURL;

String CORE_EXPORT GetStringFromTrustedType(
    const StringOrTrustedHTMLOrTrustedScriptOrTrustedScriptURLOrTrustedURL&,
    const Document*,
    ExceptionState&);

String CORE_EXPORT GetStringFromTrustedTypeWithoutCheck(
    const StringOrTrustedHTMLOrTrustedScriptOrTrustedScriptURLOrTrustedURL&);

String CORE_EXPORT GetStringFromTrustedHTML(StringOrTrustedHTML,
                                            const Document*,
                                            ExceptionState&);

String CORE_EXPORT GetStringFromTrustedScript(StringOrTrustedScript,
                                              const Document*,
                                              ExceptionState&);

String CORE_EXPORT GetStringFromTrustedScriptURL(StringOrTrustedScriptURL,
                                                 const Document*,
                                                 ExceptionState&);

String CORE_EXPORT GetStringFromTrustedURL(USVStringOrTrustedURL,
                                           const Document*,
                                           ExceptionState&);
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_TRUSTEDTYPES_TRUSTED_TYPES_UTIL_H_
