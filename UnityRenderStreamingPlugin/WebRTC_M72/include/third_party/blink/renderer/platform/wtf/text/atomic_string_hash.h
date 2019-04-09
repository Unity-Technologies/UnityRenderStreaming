/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_WTF_TEXT_ATOMIC_STRING_HASH_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_WTF_TEXT_ATOMIC_STRING_HASH_H_

#include "third_party/blink/renderer/platform/wtf/hash_traits.h"
#include "third_party/blink/renderer/platform/wtf/text/atomic_string.h"

namespace WTF {

struct AtomicStringHash {
  static unsigned GetHash(const AtomicString& key) {
    return key.Impl()->ExistingHash();
  }

  static bool Equal(const AtomicString& a, const AtomicString& b) {
    return a == b;
  }

  static const bool safe_to_compare_to_empty_or_deleted = false;
};

// AtomicStringHash is the default hash for AtomicString
template <>
struct HashTraits<AtomicString> : SimpleClassHashTraits<AtomicString> {
  // Unlike other types, we can return a const reference for AtomicString's
  // empty value (g_null_atom).
  typedef const AtomicString& PeekOutType;

  static const AtomicString& EmptyValue() { return g_null_atom; }
  static PeekOutType Peek(const AtomicString& value) { return value; }

  static const bool kHasIsEmptyValueFunction = true;
  static bool IsEmptyValue(const AtomicString& value) { return value.IsNull(); }

  static bool IsDeletedValue(const AtomicString& value) {
    return HashTraits<String>::IsDeletedValue(value.string_);
  }

  static void ConstructDeletedValue(AtomicString& slot, bool zero_value) {
    HashTraits<String>::ConstructDeletedValue(slot.string_, zero_value);
  }
};

}  // namespace WTF

using WTF::AtomicStringHash;

#endif
