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

#ifndef THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_DOCUMENT_LOADER_H_
#define THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_DOCUMENT_LOADER_H_

#include <memory>

#include "base/time/time.h"
#include "third_party/blink/public/platform/web_archive_info.h"
#include "third_party/blink/public/platform/web_common.h"
#include "third_party/blink/public/platform/web_source_location.h"
#include "third_party/blink/public/web/web_navigation_type.h"
#include "third_party/blink/public/web/web_text_direction.h"

namespace blink {

class WebDocumentSubresourceFilter;
class WebServiceWorkerNetworkProvider;
class WebURL;
class WebURLRequest;
class WebURLResponse;
template <typename T>
class WebVector;

// An interface to expose the blink::DocumentLoader to the content layer,
// including SetExtraData() and GetExtraData() to allow the content layer to
// store data that isn't relevant to Blink.
class BLINK_EXPORT WebDocumentLoader {
 public:
  class ExtraData {
   public:
    virtual ~ExtraData() = default;
  };

  // Returns the original request that resulted in this datasource.
  virtual const WebURLRequest& OriginalRequest() const = 0;

  // Returns the request corresponding to this datasource.  It may
  // include additional request headers added by WebKit that were not
  // present in the original request.  This request may also correspond
  // to a location specified by a redirect that was followed.
  virtual const WebURLRequest& GetRequest() const = 0;

  // Returns the response associated with this datasource.
  virtual const WebURLResponse& GetResponse() const = 0;

  // When this datasource was created as a result of WebFrame::loadData,
  // there may be an associated unreachableURL.
  virtual bool HasUnreachableURL() const = 0;
  virtual WebURL UnreachableURL() const = 0;

  // Allows the embedder to append redirects to the chain as a navigation
  // is starting, in case it is being transferred from another process.
  virtual void AppendRedirect(const WebURL&) = 0;

  // Returns all redirects that occurred (both client and server) before
  // at last committing the current page.  This will contain one entry
  // for each intermediate URL, and one entry for the last URL (so if
  // there are no redirects, it will contain exactly the current URL, and
  // if there is one redirect, it will contain the source and destination
  // URL).
  virtual void RedirectChain(WebVector<WebURL>&) const = 0;

  // Returns whether the navigation associated with this datasource is a
  // client redirect.
  virtual bool IsClientRedirect() const = 0;

  // Returns whether the navigation associated with this datasource should
  // replace the current history item.
  virtual bool ReplacesCurrentHistoryItem() const = 0;

  // The type of navigation that triggered the creation of this datasource.
  virtual WebNavigationType GetNavigationType() const = 0;

  // Extra data associated with this DocumentLoader.
  // Setting extra data will cause any existing extra data to be deleted.
  virtual ExtraData* GetExtraData() const = 0;
  virtual void SetExtraData(std::unique_ptr<ExtraData>) = 0;

  // Allows the embedder to inject a filter that will be consulted for each
  // subsequent subresource load, and gets the final say in deciding whether
  // or not to allow the load. The passed-in filter object is deleted when the
  // datasource is destroyed or when a new filter is set.
  virtual void SetSubresourceFilter(WebDocumentSubresourceFilter*) = 0;

  // Allows the embedder to set and return the service worker provider
  // associated with the data source. The provider may provide the service
  // worker that controls the resource loading from this data source.
  virtual void SetServiceWorkerNetworkProvider(
      std::unique_ptr<WebServiceWorkerNetworkProvider>) = 0;
  virtual WebServiceWorkerNetworkProvider*
  GetServiceWorkerNetworkProvider() = 0;

  virtual void ResetSourceLocation() = 0;

  // Can be used to temporarily suspend feeding the parser with new data. The
  // parser will be allowed to read new data when ResumeParser() is called the
  // same number of time than BlockParser().
  virtual void BlockParser() = 0;
  virtual void ResumeParser() = 0;

  // Returns info about whether the document is an MHTML archive.
  virtual bool IsArchive() const = 0;
  // Returns archive info for the archive.  Should never be called if
  // IsArchive returns false.
  virtual WebArchiveInfo GetArchiveInfo() const = 0;

  // Whether this load was started with a user gesture.
  virtual bool HadUserGesture() const = 0;

 protected:
  ~WebDocumentLoader() = default;
};

}  // namespace blink

#endif
