// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_EXT_DISJOINT_TIMER_QUERY_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_EXT_DISJOINT_TIMER_QUERY_H_

#include "third_party/blink/renderer/bindings/core/v8/script_value.h"
#include "third_party/blink/renderer/modules/webgl/webgl_extension.h"
#include "third_party/blink/renderer/platform/wtf/hash_map.h"

namespace blink {

class WebGLRenderingContextBase;
class WebGLTimerQueryEXT;

class EXTDisjointTimerQuery final : public WebGLExtension {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static EXTDisjointTimerQuery* Create(WebGLRenderingContextBase*);
  static bool Supported(WebGLRenderingContextBase*);
  static const char* ExtensionName();

  explicit EXTDisjointTimerQuery(WebGLRenderingContextBase*);

  WebGLExtensionName GetName() const override;

  WebGLTimerQueryEXT* createQueryEXT();
  void deleteQueryEXT(WebGLTimerQueryEXT*);
  GLboolean isQueryEXT(WebGLTimerQueryEXT*);
  void beginQueryEXT(GLenum, WebGLTimerQueryEXT*);
  void endQueryEXT(GLenum);
  void queryCounterEXT(WebGLTimerQueryEXT*, GLenum);
  ScriptValue getQueryEXT(ScriptState*, GLenum, GLenum);
  ScriptValue getQueryObjectEXT(ScriptState*, WebGLTimerQueryEXT*, GLenum);

  void Trace(blink::Visitor*) override;

 private:
  friend class WebGLTimerQueryEXT;

  Member<WebGLTimerQueryEXT> current_elapsed_query_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBGL_EXT_DISJOINT_TIMER_QUERY_H_
