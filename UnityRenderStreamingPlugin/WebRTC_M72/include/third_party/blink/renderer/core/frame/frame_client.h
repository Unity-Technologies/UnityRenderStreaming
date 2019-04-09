// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_FRAME_CLIENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_FRAME_CLIENT_H_

#include "base/unguessable_token.h"
#include "third_party/blink/public/platform/blame_context.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class Frame;
enum class FrameDetachType;

class CORE_EXPORT FrameClient : public GarbageCollectedFinalized<FrameClient> {
 public:
  virtual bool InShadowTree() const = 0;

  virtual void Detached(FrameDetachType) = 0;

  virtual Frame* Opener() const = 0;
  virtual void SetOpener(Frame*) = 0;

  virtual Frame* Parent() const = 0;
  virtual Frame* Top() const = 0;
  virtual Frame* NextSibling() const = 0;
  virtual Frame* FirstChild() const = 0;

  virtual unsigned BackForwardLength() = 0;

  virtual void FrameFocused() const = 0;

  virtual base::UnguessableToken GetDevToolsFrameToken() const = 0;

  virtual ~FrameClient() = default;

  virtual void Trace(blink::Visitor* visitor) {}
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_FRAME_CLIENT_H_
