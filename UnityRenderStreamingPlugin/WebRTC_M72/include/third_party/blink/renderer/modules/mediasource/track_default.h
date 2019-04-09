// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIASOURCE_TRACK_DEFAULT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIASOURCE_TRACK_DEFAULT_H_

#include "third_party/blink/renderer/bindings/core/v8/script_value.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class ExceptionState;
class ScriptState;

class TrackDefault final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static const AtomicString& AudioKeyword();
  static const AtomicString& VideoKeyword();
  static const AtomicString& TextKeyword();

  static TrackDefault* Create(const AtomicString& type,
                              const String& language,
                              const String& label,
                              const Vector<String>& kinds,
                              const String& byte_stream_track_id,
                              ExceptionState&);

  ~TrackDefault() override;

  // Implement the IDL
  AtomicString type() const { return type_; }
  String byteStreamTrackID() const { return byte_stream_track_id_; }
  String language() const { return language_; }
  String label() const { return label_; }
  ScriptValue kinds(ScriptState*) const;

 private:
  TrackDefault(const AtomicString& type,
               const String& language,
               const String& label,
               const Vector<String>& kinds,
               const String& byte_stream_track_id);

  const AtomicString type_;
  const String byte_stream_track_id_;
  const String language_;
  const String label_;
  const Vector<String> kinds_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIASOURCE_TRACK_DEFAULT_H_
