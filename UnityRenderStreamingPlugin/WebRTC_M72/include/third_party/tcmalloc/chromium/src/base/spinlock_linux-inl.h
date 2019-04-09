// -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
/* Copyright (c) 2009, Google Inc.
 * All rights reserved.
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
 *
 * ---
 * This file is a Linux-specific part of spinlock_internal.cc
 */

#include <errno.h>
#include <sched.h>
#include <time.h>
#include <limits.h>
#include "base/linux_syscall_support.h"

#define FUTEX_WAIT 0
#define FUTEX_WAKE 1
#define FUTEX_PRIVATE_FLAG 128

// Note: Instead of making direct system calls that are inlined, we rely
//       on the syscall() function in glibc to do the right thing. This
//       is necessary to make the code compatible with the seccomp sandbox,
//       which needs to be able to find and patch all places where system
//       calls are made. Scanning through and patching glibc is fast, but
//       doing so on the entire Chrome binary would be prohibitively
//       expensive.
//       This is a notable change from the upstream version of tcmalloc,
//       which prefers direct system calls in order to improve compatibility
//       with older toolchains and runtime libraries.

namespace base {
namespace internal {

void SpinLockDelay(volatile Atomic32 *w, int32 value, int loop) {
  if (loop != 0) {
    int save_errno = errno;
    struct timespec tm;
    tm.tv_sec = 0;
    tm.tv_nsec = base::internal::SuggestedDelayNS(loop);
    tm.tv_nsec *= 16;  // increase the delay; we expect explicit wakeups
    syscall(__NR_futex, reinterpret_cast<int*>(const_cast<Atomic32*>(w)),
            FUTEX_WAIT | FUTEX_PRIVATE_FLAG, value,
            reinterpret_cast<struct kernel_timespec*>(&tm), NULL, 0);
    errno = save_errno;
  }
}

void SpinLockWake(volatile Atomic32 *w, bool all) {
  syscall(__NR_futex, reinterpret_cast<int*>(const_cast<Atomic32*>(w)),
          FUTEX_WAKE | FUTEX_PRIVATE_FLAG, all ? INT_MAX : 1, NULL, NULL, 0);
}

} // namespace internal
} // namespace base
