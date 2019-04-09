// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_INSPECTOR_RESOLVE_NODE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_INSPECTOR_RESOLVE_NODE_H_

#include <memory>

#include "third_party/blink/renderer/core/inspector/protocol/Runtime.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "v8/include/v8-inspector.h"

namespace blink {

class LocalFrame;
class Node;

v8::Local<v8::Value> NodeV8Value(v8::Local<v8::Context>, Node*);

std::unique_ptr<v8_inspector::protocol::Runtime::API::RemoteObject> ResolveNode(
    v8_inspector::V8InspectorSession*,
    Node*,
    const String& object_group);

std::unique_ptr<v8_inspector::protocol::Runtime::API::RemoteObject>
NullRemoteObject(v8_inspector::V8InspectorSession* v8_session,
                 LocalFrame*,
                 const String& object_group);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_INSPECTOR_RESOLVE_NODE_H_
