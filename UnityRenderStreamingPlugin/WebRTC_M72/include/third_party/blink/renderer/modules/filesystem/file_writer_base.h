/*
 * Copyright (C) 2010 Google Inc.  All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_FILESYSTEM_FILE_WRITER_BASE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_FILESYSTEM_FILE_WRITER_BASE_H_

#include <memory>
#include "base/files/file.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"

namespace blink {

class MODULES_EXPORT FileWriterBase : public GarbageCollectedMixin {
 public:
  virtual ~FileWriterBase();
  void Initialize(const KURL& path, long long length);

  long long position() const { return position_; }
  long long length() const { return length_; }

  void Trace(blink::Visitor* visitor) override {}

  virtual void Truncate(long long length);
  virtual void Write(long long position, const String& id);
  virtual void Cancel();

 protected:
  FileWriterBase();

  void SetPosition(long long position) { position_ = position; }

  void SetLength(long long length) { length_ = length; }

  void SeekInternal(long long position);

  // This calls DidSucceed() or DidFail() based on the value of |error_code|.
  void DidFinish(base::File::Error error_code);
  void DidSucceed();
  void DidWrite(int64_t bytes, bool complete);
  void DidFail(base::File::Error error_code);

  // Derived classes must provide these methods to asynchronously perform
  // the requested operation, and they must call the appropriate DidSomething
  // method upon completion and as progress is made in the Write case.
  virtual void DoTruncate(const KURL& path, int64_t offset) = 0;
  virtual void DoWrite(const KURL& path,
                       const String& blob_id,
                       int64_t offset) = 0;
  virtual void DoCancel() = 0;

  // These are conditionally called by the Did* methods.
  virtual void DidWriteImpl(int64_t bytes, bool complete) = 0;
  virtual void DidFailImpl(base::File::Error error_code) = 0;
  virtual void DidTruncateImpl() = 0;

 private:
  enum OperationType { kOperationNone, kOperationWrite, kOperationTruncate };

  enum CancelState {
    kCancelNotInProgress,
    kCancelSent,
    kCancelReceivedWriteResponse,
  };

  void FinishCancel();

  long long position_;
  long long length_;
  KURL path_;
  OperationType operation_;
  CancelState cancel_state_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_FILESYSTEM_FILE_WRITER_BASE_H_
