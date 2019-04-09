// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_LOADER_FETCH_RESOURCE_LOADING_LOG_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_LOADER_FETCH_RESOURCE_LOADING_LOG_H_

#include "third_party/blink/renderer/platform/wtf/assertions.h"

#if DCHECK_IS_ON()
// We can see logs with |--v=N| or |--vmodule=ResourceLoadingLog=N| where N is a
// verbose level.
#define RESOURCE_LOADING_DVLOG(verbose_level) \
  LAZY_STREAM(                                \
      VLOG_STREAM(verbose_level),             \
      ((verbose_level) <= ::logging::GetVlogLevel("ResourceLoadingLog.h")))
#else
#define RESOURCE_LOADING_DVLOG(verbose_level) EAT_STREAM_PARAMETERS
#endif

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_LOADER_FETCH_RESOURCE_LOADING_LOG_H_
