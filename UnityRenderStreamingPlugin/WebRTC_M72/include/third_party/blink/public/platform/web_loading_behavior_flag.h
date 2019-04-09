// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_LOADING_BEHAVIOR_FLAG_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_LOADING_BEHAVIOR_FLAG_H_

namespace blink {

// This enum tracks certain behavior Blink exhibits when loading a page. This is
// for use in metrics collection by the loading team, to evaluate experimental
// features and potential areas of improvement in the loading stack. The main
// consumer is the page_load_metrics component, which sends bit flags to the
// browser process for histogram splitting.
enum WebLoadingBehaviorFlag {
  kWebLoadingBehaviorNone = 0,
  // Indicates that the page used the document.write evaluator to preload scan
  // for resources inserted via document.write.
  // DEPRECATED, feature has been turned down.
  kWebLoadingBehaviorDocumentWriteEvaluator = 1 << 0,
  // Indicates that the page is controlled by a Service Worker.
  kWebLoadingBehaviorServiceWorkerControlled = 1 << 1,
  // Indicates that the page has a synchronous, cross-origin document.written
  // script.
  kWebLoadingBehaviorDocumentWriteBlock = 1 << 2,
  // Indicates that the page is a reload and has a synchronous, cross-origin
  // document.written script.
  kWebLoadingBehaviorDocumentWriteBlockReload = 1 << 3,
  // The page loaded external CSS that generated a PreloadRequest via the
  // CSSPreloaderResourceClient.
  kWebLoadingBehaviorCSSPreloadFound = 1 << 4,
  // Indicates that the page has a synchronous, same-origin document.written
  // script with different protocol.
  kWebLoadingBehaviorDocumentWriteBlockDifferentScheme = 1 << 5,
  // Indicates that a subresource on the page matched the subresource filtering
  // rules.
  kWebLoadingBehaviorSubresourceFilterMatch = 1 << 6,
  // Indicates that a subresource on the page was a candidate for stale
  // while revalidate and caused a network load.
  kStaleWhileRevalidateResourceCandidateNetworkLoad = 1 << 7,
  // Indicates that a subresource on the page was a candidate for stale
  // while revalidate and caused a stale cache load.
  kStaleWhileRevalidateResourceCandidateStaleCacheLoad = 1 << 8,
  // Indicates that a subresource on the page was a candidate for stale
  // while revalidate and caused a non-stale cache load.
  kStaleWhileRevalidateResourceCandidateCacheLoad = 1 << 9,
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_LOADING_BEHAVIOR_FLAG_H_
