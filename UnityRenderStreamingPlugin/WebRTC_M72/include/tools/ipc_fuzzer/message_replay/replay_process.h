// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TOOLS_IPC_FUZZER_REPLAY_REPLAY_PROCESS_H_
#define TOOLS_IPC_FUZZER_REPLAY_REPLAY_PROCESS_H_

#include <stddef.h>

#include <memory>

#include "base/macros.h"
#include "base/message_loop/message_loop.h"
#include "base/synchronization/waitable_event.h"
#include "base/threading/thread.h"
#include "base/timer/timer.h"
#include "ipc/ipc_channel_proxy.h"
#include "ipc/ipc_listener.h"
#include "ipc/ipc_message.h"
#include "tools/ipc_fuzzer/message_lib/message_file.h"

namespace content {
class ServiceManagerConnection;
}

namespace mojo {
class IncomingInvitation;
namespace core {
class ScopedIPCSupport;
}  // namespace core
}  // namespace mojo

namespace ipc_fuzzer {

class ReplayProcess : public IPC::Listener {
 public:
  ReplayProcess();
  ~ReplayProcess() override;

  // Set up command line, logging, IO thread. Returns true on success, false
  // otherwise.
  bool Initialize(int argc, const char** argv);

  // Open a channel to the browser process. It will think we are a renderer.
  void OpenChannel();

  // Extract messages from a file specified by --ipc-fuzzer-testcase=
  // Returns true on success, false otherwise.
  bool OpenTestcase();

  // Send messages to the browser.
  void Run();

  // IPC::Listener implementation.
  bool OnMessageReceived(const IPC::Message& message) override;
  void OnChannelError() override;

 private:
  void SendNextMessage();

  std::unique_ptr<mojo::core::ScopedIPCSupport> mojo_ipc_support_;
  std::unique_ptr<mojo::IncomingInvitation> mojo_invitation_;
  std::unique_ptr<content::ServiceManagerConnection>
      service_manager_connection_;
  std::unique_ptr<IPC::ChannelProxy> channel_;
  base::MessageLoop main_loop_;
  base::Thread io_thread_;
  base::WaitableEvent shutdown_event_;
  MessageVector messages_;
  size_t message_index_;

  DISALLOW_COPY_AND_ASSIGN(ReplayProcess);
};

}  // namespace ipc_fuzzer

#endif  // TOOLS_IPC_FUZZER_REPLAY_REPLAY_PROCESS_H_
