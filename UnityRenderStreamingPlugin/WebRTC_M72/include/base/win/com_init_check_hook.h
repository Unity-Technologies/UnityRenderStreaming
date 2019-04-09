// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_WIN_COM_INIT_CHECK_HOOK_H_
#define BASE_WIN_COM_INIT_CHECK_HOOK_H_

#include "base/base_export.h"
#include "base/logging.h"
#include "base/macros.h"
#include "build/build_config.h"

namespace base {
namespace win {

// Hotpatching is only supported in Intel 32-bit x86 processors because Windows
// binaries contain a convenient 2 byte hotpatch noop. This doesn't exist in
// 64-bit binaries.

#if DCHECK_IS_ON() && defined(ARCH_CPU_X86_FAMILY) &&             \
    defined(ARCH_CPU_32_BITS) && !defined(GOOGLE_CHROME_BUILD) && \
    !defined(OFFICIAL_BUILD) &&                                   \
    !defined(COM_INIT_CHECK_HOOK_DISABLED)  // See crbug/737090 for details.
#define COM_INIT_CHECK_HOOK_ENABLED
#endif

// Manages the installation of consistency DCHECK hooks of COM APIs that require
// COM to be initialized and only works if COM_INIT_CHECK_HOOK_ENABLED is
// defined. Care should be taken if this is instantiated with multiple threads
// running as the hotpatch does not apply atomically.
class BASE_EXPORT ComInitCheckHook {
 public:
  ComInitCheckHook();
  ~ComInitCheckHook();

 private:
  DISALLOW_COPY_AND_ASSIGN(ComInitCheckHook);
};

}  // namespace win
}  // namespace base

#endif  // BASE_WIN_COM_INIT_CHECK_HOOK_H_
