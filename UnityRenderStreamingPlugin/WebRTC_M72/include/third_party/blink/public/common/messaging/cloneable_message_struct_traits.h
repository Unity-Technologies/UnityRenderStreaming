// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_COMMON_MESSAGING_CLONEABLE_MESSAGE_STRUCT_TRAITS_H_
#define THIRD_PARTY_BLINK_PUBLIC_COMMON_MESSAGING_CLONEABLE_MESSAGE_STRUCT_TRAITS_H_

#include "mojo/public/cpp/base/big_buffer.h"
#include "mojo/public/cpp/base/unguessable_token_mojom_traits.h"
#include "third_party/blink/public/common/messaging/cloneable_message.h"
#include "third_party/blink/public/mojom/messaging/cloneable_message.mojom.h"

namespace mojo {

template <>
struct BLINK_COMMON_EXPORT
    StructTraits<blink::mojom::CloneableMessage::DataView,
                 blink::CloneableMessage> {
  static mojo_base::BigBufferView encoded_message(
      blink::CloneableMessage& input);

  static std::vector<blink::mojom::SerializedBlobPtr>& blobs(
      blink::CloneableMessage& input) {
    return input.blobs;
  }

  static uint64_t stack_trace_id(const blink::CloneableMessage& input) {
    return input.stack_trace_id;
  }

  static int64_t stack_trace_debugger_id_first(
      const blink::CloneableMessage& input) {
    return input.stack_trace_debugger_id_first;
  }

  static int64_t stack_trace_debugger_id_second(
      const blink::CloneableMessage& input) {
    return input.stack_trace_debugger_id_second;
  }

  static const base::Optional<base::UnguessableToken>& locked_agent_cluster_id(
      const blink::CloneableMessage& input) {
    return input.locked_agent_cluster_id;
  }

  static bool Read(blink::mojom::CloneableMessage::DataView data,
                   blink::CloneableMessage* out);
};

}  // namespace mojo

#endif  // THIRD_PARTY_BLINK_PUBLIC_COMMON_MESSAGING_CLONEABLE_MESSAGE_STRUCT_TRAITS_H_
