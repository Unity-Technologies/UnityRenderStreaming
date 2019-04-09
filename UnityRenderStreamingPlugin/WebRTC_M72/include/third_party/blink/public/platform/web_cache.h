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

#ifndef THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_CACHE_H_
#define THIRD_PARTY_BLINK_PUBLIC_PLATFORM_WEB_CACHE_H_

#include "third_party/blink/public/platform/web_common.h"

namespace blink {

// An interface to query and configure Blink's resource cache.
class WebCache {
 public:
  struct UsageStats {
    size_t capacity;
    size_t size;
  };

  // A struct mirroring blink::MemoryCache::TypeStatistic.
  struct ResourceTypeStat {
    size_t count;
    size_t size;
    size_t decoded_size;
  };

  // A struct mirroring blink::MemoryCache::Statistics.
  struct ResourceTypeStats {
    ResourceTypeStat images;
    ResourceTypeStat css_style_sheets;
    ResourceTypeStat scripts;
    ResourceTypeStat xsl_style_sheets;
    ResourceTypeStat fonts;
    ResourceTypeStat other;
  };

  // Sets the capacities of the resource cache, evicting objects as necessary.
  BLINK_PLATFORM_EXPORT static void SetCapacity(size_t);

  // Clears the cache (as much as possible; some resources may not be
  // cleared if they are actively referenced). Note that this method
  // only removes resources from live list, w/o releasing cache memory.
  BLINK_PLATFORM_EXPORT static void Clear();

  // Gets the usage statistics from the resource cache.
  BLINK_PLATFORM_EXPORT static void GetUsageStats(UsageStats*);

  // Get usage stats about the resource cache.
  BLINK_PLATFORM_EXPORT static void GetResourceTypeStats(ResourceTypeStats*);

 private:
  WebCache() = delete;  // Not intended to be instanced.
};

}  // namespace blink

#endif
