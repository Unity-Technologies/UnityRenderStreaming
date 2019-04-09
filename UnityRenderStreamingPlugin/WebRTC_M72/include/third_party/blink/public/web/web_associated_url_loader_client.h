// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_ASSOCIATED_URL_LOADER_CLIENT_H_
#define THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_ASSOCIATED_URL_LOADER_CLIENT_H_

namespace blink {

class WebURL;
class WebURLResponse;
struct WebURLError;

class WebAssociatedURLLoaderClient {
 public:
  virtual bool WillFollowRedirect(const WebURL& new_url,
                                  const WebURLResponse& redirect_response) {
    return true;
  }
  virtual void DidSendData(unsigned long long bytes_sent,
                           unsigned long long total_bytes_to_be_sent) {}
  virtual void DidReceiveResponse(const WebURLResponse&) {}
  virtual void DidDownloadData(int data_length) {}
  virtual void DidReceiveData(const char* data, int data_length) {}
  virtual void DidReceiveCachedMetadata(const char* data, int data_length) {}
  virtual void DidFinishLoading() {}
  virtual void DidFail(const WebURLError&) {}

 protected:
  virtual ~WebAssociatedURLLoaderClient() = default;
};

}  // namespace blink

#endif
