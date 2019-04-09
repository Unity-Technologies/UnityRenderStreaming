// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebURLLoaderMockFactoryImpl_h
#define WebURLLoaderMockFactoryImpl_h

#include <map>

#include "base/files/file_path.h"
#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "base/optional.h"
#include "third_party/blink/public/platform/web_url.h"
#include "third_party/blink/public/platform/web_url_error.h"
#include "third_party/blink/public/platform/web_url_loader_mock_factory.h"
#include "third_party/blink/public/platform/web_url_request.h"
#include "third_party/blink/public/platform/web_url_response.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/weborigin/kurl_hash.h"
#include "third_party/blink/renderer/platform/wtf/hash_map.h"

namespace blink {

class TestingPlatformSupport;
class WebData;
class WebURLLoader;
class WebURLLoaderMock;
class WebURLLoaderTestDelegate;

// A factory that creates WebURLLoaderMock to simulate resource loading in
// tests. Since there are restriction and rules to follow, please read comments
// in WebURLLoaderMockFactory carefully to use this class correctly.
class WebURLLoaderMockFactoryImpl : public WebURLLoaderMockFactory {
 public:
  WebURLLoaderMockFactoryImpl(TestingPlatformSupport*);
  ~WebURLLoaderMockFactoryImpl() override;

  // WebURLLoaderMockFactory:
  std::unique_ptr<WebURLLoader> CreateURLLoader(
      std::unique_ptr<WebURLLoader> default_loader) override;
  void RegisterURL(const WebURL& url,
                   const WebURLResponse& response,
                   const WebString& file_path = WebString()) override;
  void RegisterErrorURL(const WebURL& url,
                        const WebURLResponse& response,
                        const WebURLError& error) override;
  void UnregisterURL(const WebURL& url) override;
  void RegisterURLProtocol(const WebString& protocol,
                           const WebURLResponse& response,
                           const WebString& file_path) override;
  void UnregisterURLProtocol(const WebString& protocol) override;
  void UnregisterAllURLsAndClearMemoryCache() override;
  void ServeAsynchronousRequests() override;
  void SetLoaderDelegate(WebURLLoaderTestDelegate* delegate) override {
    delegate_ = delegate;
  }

  // Returns true if |url| was registered for being mocked.
  bool IsMockedURL(const WebURL& url);

  // Called by the loader to load a resource.
  void LoadSynchronously(const WebURLRequest& request,
                         WebURLResponse* response,
                         base::Optional<WebURLError>* error,
                         WebData* data,
                         int64_t* encoded_data_length);
  void LoadAsynchronouly(const WebURLRequest& request,
                         WebURLLoaderMock* loader);

  // Removes the loader from the list of pending loaders.
  void CancelLoad(WebURLLoaderMock* loader);

 private:
  struct ResponseInfo {
    WebURLResponse response;
    base::FilePath file_path;
  };

  virtual void RunUntilIdle();

  // Loads the specified request and populates the response, error and data
  // accordingly.
  void LoadRequest(const WebURL& url,
                   WebURLResponse* response,
                   base::Optional<WebURLError>* error,
                   WebData* data);

  // Checks if the loader is pending. Otherwise, it may have been deleted.
  bool IsPending(base::WeakPtr<WebURLLoaderMock> loader);

  // Looks up an URL in the mock URL table.
  //
  // If the URL is found, returns true and sets |error| and |response_info|.
  bool LookupURL(const WebURL& url,
                 base::Optional<WebURLError>* error,
                 ResponseInfo* response_info);

  // Reads |m_filePath| and puts its content in |data|.
  // Returns true if it successfully read the file.
  static bool ReadFile(const base::FilePath& file_path, WebData* data);

  WebURLLoaderTestDelegate* delegate_ = nullptr;

  // The loaders that have not being served data yet.
  using LoaderToRequestMap = HashMap<WebURLLoaderMock*, WebURLRequest>;
  LoaderToRequestMap pending_loaders_;

  // All values must be valid, but we use Optional because HashMap requires
  // "empty value".
  typedef HashMap<KURL, base::Optional<WebURLError>> URLToErrorMap;
  URLToErrorMap url_to_error_info_;

  // Table of the registered URLs and the responses that they should receive.
  using URLToResponseMap = HashMap<KURL, ResponseInfo>;
  URLToResponseMap url_to_response_info_;

  // Table of the registered URL protocols and the responses that they should
  // receive.
  using ProtocolToResponseMap = HashMap<String, ResponseInfo>;
  ProtocolToResponseMap protocol_to_response_info_;

  TestingPlatformSupport* platform_;

  DISALLOW_COPY_AND_ASSIGN(WebURLLoaderMockFactoryImpl);
};

}  // namespace blink

#endif  // WebURLLoaderMockFactoryImpl_h
