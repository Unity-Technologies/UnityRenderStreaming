/*
 * Copyright (C) 2006 Apple Inc.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_NETWORK_MIME_MIME_TYPE_REGISTRY_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_NETWORK_MIME_MIME_TYPE_REGISTRY_H_

#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/hash_set.h"
#include "third_party/blink/renderer/platform/wtf/text/string_hash.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

// Note/reminder: MIME type and parameter names are per-RFC case
// insensitive (https://www.ietf.org/rfc/rfc2045.txt , section 5.1).
// The MIMETypeRegistry predicates are all case-insensitive.
class PLATFORM_EXPORT MIMETypeRegistry {
  STATIC_ONLY(MIMETypeRegistry);

 public:
  // For Media MIME type checks.
  enum SupportsType { kIsNotSupported, kIsSupported, kMayBeSupported };

  static String GetMIMETypeForExtension(const String& extension);
  static String GetWellKnownMIMETypeForExtension(const String& extension);

  // Checks to see if the given mime type is supported.
  static bool IsSupportedMIMEType(const String& mime_type);

  // Checks to see if a mime type is suitable for being loaded inline as an
  // image (e.g., <img> tags).
  static bool IsSupportedImageMIMEType(const String& mime_type);

  // Checks to see if a mime type is suitable for being loaded as an image
  // document in a frame.
  static bool IsSupportedImageResourceMIMEType(const String& mime_type);

  // Checks to see if a mime type is suitable for being displayed as an image.
  static bool IsSupportedImagePrefixedMIMEType(const String& mime_type);

  // Checks to see if a mime type is suitable for being encoded.
  static bool IsSupportedImageMIMETypeForEncoding(const String& mime_type);

  // Checks to see if a mime type is one of the default modern formats supported
  // when the 'legacy-image-formats' feature is disabled.
  static bool IsModernImageMIMEType(const String& mime_type);

  // Checks to see if a mime type is suitable for being loaded as a JavaScript
  // resource.
  static bool IsSupportedJavaScriptMIMEType(const String& mime_type);

  static bool IsLegacySupportedJavaScriptLanguage(const String& language);

  // Checks to see if a non-image mime type is suitable for being loaded as a
  // document in a frame. Includes supported JavaScript MIME types.
  static bool IsSupportedNonImageMIMEType(const String& mime_type);

  // Checks to see if the mime type and codecs are supported media MIME types.
  static bool IsSupportedMediaMIMEType(const String& mime_type,
                                       const String& codecs);

  // Does similar to isSupportedMediaMIMEType, but returns a little more
  // detailed information in SupportsType enum.
  static SupportsType SupportsMediaMIMEType(const String& mime_type,
                                            const String& codecs);

  // Checks to see if the mime type and codecs are supported by the MediaSource
  // implementation.
  static bool IsSupportedMediaSourceMIMEType(const String& mime_type,
                                             const String& codecs);

  // Checks to see if a mime type is a valid Java applet mime type
  static bool IsJavaAppletMIMEType(const String& mime_type);

  // Checks to see if a mime type is suitable for being loaded as a stylesheet.
  static bool IsSupportedStyleSheetMIMEType(const String& mime_type);

  // Checks to see if a mime type is suitable for being loaded as a font.
  static bool IsSupportedFontMIMEType(const String& mime_type);

  // Checks to see if a mime type is suitable for being loaded as a text track.
  static bool IsSupportedTextTrackMIMEType(const String& mime_type);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_NETWORK_MIME_MIME_TYPE_REGISTRY_H_
