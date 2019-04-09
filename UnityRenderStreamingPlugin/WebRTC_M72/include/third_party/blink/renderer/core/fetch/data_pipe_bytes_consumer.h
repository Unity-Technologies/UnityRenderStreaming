// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FETCH_DATA_PIPE_BYTES_CONSUMER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FETCH_DATA_PIPE_BYTES_CONSUMER_H_

#include <memory>

#include "mojo/public/cpp/system/data_pipe.h"
#include "mojo/public/cpp/system/simple_watcher.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/fetch/bytes_consumer.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class ExecutionContext;

// An adapter for mojo::DataPipe. As mojo::DataPipe lacks signals completion and
// error signals, we define another interface, CompletionNotifier, for the
// signals.
class CORE_EXPORT DataPipeBytesConsumer final : public BytesConsumer {
 public:
  class CORE_EXPORT CompletionNotifier final
      : public GarbageCollected<CompletionNotifier> {
   public:
    explicit CompletionNotifier(DataPipeBytesConsumer* bytes_consumer)
        : bytes_consumer_(bytes_consumer) {}

    // One of these methods must be called to signal the end of the data
    // stream.  We cannot assume that the end of the pipe completes the
    // stream successfully since errors can occur after the last byte is
    // written into the pipe.
    void SignalComplete();
    void SignalError(const BytesConsumer::Error& error);
    void Trace(blink::Visitor*);

   private:
    const WeakMember<DataPipeBytesConsumer> bytes_consumer_;
  };

  DataPipeBytesConsumer(ExecutionContext*,
                        mojo::ScopedDataPipeConsumerHandle,
                        CompletionNotifier** notifier);
  ~DataPipeBytesConsumer() override;

  Result BeginRead(const char** buffer, size_t* available) override;
  Result EndRead(size_t read_size) override;
  mojo::ScopedDataPipeConsumerHandle DrainAsDataPipe() override;
  void SetClient(BytesConsumer::Client*) override;
  void ClearClient() override;

  void Cancel() override;
  PublicState GetPublicState() const override;
  Error GetError() const override {
    DCHECK(state_ == InternalState::kErrored);
    return error_;
  }
  String DebugName() const override { return "DataPipeBytesConsumer"; }

  void Trace(blink::Visitor*) override;

 private:
  bool IsReadableOrWaiting() const;
  void MaybeClose();
  void SetError(const Error& error);
  void Notify(MojoResult);
  void ClearDataPipe();
  void SignalComplete();
  void SignalError(const Error& error);

  Member<ExecutionContext> execution_context_;
  mojo::ScopedDataPipeConsumerHandle data_pipe_;
  mojo::SimpleWatcher watcher_;
  Member<BytesConsumer::Client> client_;
  InternalState state_ = InternalState::kWaiting;
  Error error_;
  bool is_in_two_phase_read_ = false;
  bool has_pending_notification_ = false;
  bool has_pending_complete_ = false;
  bool has_pending_error_ = false;
  bool completion_signaled_ = false;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FETCH_DATA_PIPE_BYTES_CONSUMER_H_
