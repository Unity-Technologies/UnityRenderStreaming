// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MAC_DISPATCH_SOURCE_MACH_H_
#define BASE_MAC_DISPATCH_SOURCE_MACH_H_

#include <dispatch/dispatch.h>

#include "base/base_export.h"
#include "base/mac/scoped_dispatch_object.h"
#include "base/macros.h"

namespace base {

// This class encapsulates a MACH_RECV dispatch source. When this object is
// destroyed, the source will be cancelled and it will wait for the source
// to stop executing work. The source can run on either a user-supplied queue,
// or it can create its own for the source.
class BASE_EXPORT DispatchSourceMach {
 public:
  // Creates a new dispatch source for the |port| and schedules it on a new
  // queue that will be created with |name|. When a Mach message is received,
  // the |event_handler| will be called.
  DispatchSourceMach(const char* name,
                     mach_port_t port,
                     void (^event_handler)());

  // Creates a new dispatch source with the same semantics as above, but rather
  // than creating a new queue, it schedules the source on |queue|.
  DispatchSourceMach(dispatch_queue_t queue,
                     mach_port_t port,
                     void (^event_handler)());

  // Cancels the source and waits for it to become fully cancelled before
  // releasing the source.
  ~DispatchSourceMach();

  // Resumes the source. This must be called before any Mach messages will
  // be received.
  void Resume();

 private:
  // The dispatch queue used to service the source_.
  ScopedDispatchObject<dispatch_queue_t> queue_;

  // A MACH_RECV dispatch source.
  ScopedDispatchObject<dispatch_source_t> source_;

  // Semaphore used to wait on the |source_|'s cancellation in the destructor.
  ScopedDispatchObject<dispatch_semaphore_t> source_canceled_;

  DISALLOW_COPY_AND_ASSIGN(DispatchSourceMach);
};

}  // namespace base

#endif  // BASE_MAC_DISPATCH_SOURCE_MACH_H_
