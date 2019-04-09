/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_THREADABLE_LOADER_CLIENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_THREADABLE_LOADER_CLIENT_H_

#include <memory>

#include "base/macros.h"
#include "third_party/blink/public/platform/web_data_consumer_handle.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/blob/blob_data.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class KURL;
class ResourceError;
class ResourceResponse;
class ResourceTimingInfo;

class CORE_EXPORT ThreadableLoaderClient : public GarbageCollectedMixin {
 public:
  virtual void DidSendData(unsigned long long /*bytesSent*/,
                           unsigned long long /*totalBytesToBeSent*/) {}
  // Note that redirects for redirect modes kError and kManual are still
  // notified here. A client must return false in such cases.
  virtual bool WillFollowRedirect(const KURL& new_url,
                                  const ResourceResponse&) {
    return true;
  }
  virtual void DidReceiveResponse(unsigned long /*identifier*/,
                                  const ResourceResponse&,
                                  std::unique_ptr<WebDataConsumerHandle>) {}
  virtual void DidReceiveData(const char*, unsigned /*dataLength*/) {}
  virtual void DidReceiveCachedMetadata(const char*, int /*dataLength*/) {}
  virtual void DidFinishLoading(unsigned long /*identifier*/) {}
  virtual void DidFail(const ResourceError&) {}
  virtual void DidFailRedirectCheck() {}
  virtual void DidReceiveResourceTiming(const ResourceTimingInfo&) {}

  virtual void DidDownloadData(int /*dataLength*/) {}
  // Called for requests that had DownloadToBlob set to true. Can be called with
  // null if creating the blob failed for some reason (but the download itself
  // otherwise succeeded). Could also not be called at all if the downloaded
  // resource ended up being zero bytes.
  virtual void DidDownloadToBlob(scoped_refptr<BlobDataHandle>) {}

  virtual ~ThreadableLoaderClient() = default;

 protected:
  ThreadableLoaderClient() = default;

  DISALLOW_COPY_AND_ASSIGN(ThreadableLoaderClient);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_THREADABLE_LOADER_CLIENT_H_
