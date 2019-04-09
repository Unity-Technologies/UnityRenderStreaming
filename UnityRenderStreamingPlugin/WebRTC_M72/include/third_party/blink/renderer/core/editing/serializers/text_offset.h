// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_SERIALIZERS_TEXT_OFFSET_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_SERIALIZERS_TEXT_OFFSET_H_

#include "third_party/blink/public/platform/platform.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

namespace blink {

class Text;

class TextOffset {
  STACK_ALLOCATED();

 public:
  TextOffset();
  TextOffset(Text*, int);
  TextOffset(const TextOffset&);

  Text* GetText() const { return text_.Get(); }
  int Offset() const { return offset_; }

  bool IsNull() const;
  bool IsNotNull() const;

 private:
  Member<Text> text_;
  int offset_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_SERIALIZERS_TEXT_OFFSET_H_
