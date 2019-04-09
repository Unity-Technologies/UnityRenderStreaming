// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is Chromium-specific, and brings in the appropriate
// event-config.h depending on your platform.

#if defined(__native_client_nonsfi__)
#include "base/third_party/libevent/nacl_nonsfi/event-config.h"
#elif defined(__APPLE__)
#include "base/third_party/libevent/mac/event-config.h"
#elif defined(ANDROID)
#include "base/third_party/libevent/android/event-config.h"
#elif defined(__linux__)
#include "base/third_party/libevent/linux/event-config.h"
#elif defined(__FreeBSD__)
#include "base/third_party/libevent/freebsd/event-config.h"
#elif defined(__sun)
#include "base/third_party/libevent/solaris/event-config.h"
#elif defined(_AIX)
#include "base/third_party/libevent/aix/event-config.h"
#else
#error generate event-config.h for your platform
#endif
