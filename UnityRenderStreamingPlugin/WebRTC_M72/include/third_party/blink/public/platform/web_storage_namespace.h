/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_STORAGE_NAMESPACE_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_STORAGE_NAMESPACE_H_

#include "third_party/blink/public/platform/web_common.h"
#include "third_party/blink/public/platform/web_string.h"

namespace blink {

class WebStorageArea;
class WebSecurityOrigin;

// WebStorageNamespace represents a collection of StorageAreas. Typically,
// you'll have multiple StorageNamespaces to represent the SessionStorage for
// each tab and a single StorageNamespace to represent LocalStorage for the
// entire browser.
class WebStorageNamespace {
 public:
  virtual ~WebStorageNamespace() = default;

  // Create a new WebStorageArea object. Two subsequent calls with the same
  // origin will return two different WebStorageArea objects that share the same
  // backing store.  You should call delete on the returned object when you're
  // finished.
  virtual WebStorageArea* CreateStorageArea(const WebSecurityOrigin&) = 0;

  virtual WebString GetNamespaceId() const = 0;

  // Returns true of the two instances represent the same storage namespace.
  virtual bool IsSameNamespace(const WebStorageNamespace&) const {
    return false;
  }
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_STORAGE_NAMESPACE_H_
