/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_STREAM_H_
#define RTC_BASE_STREAM_H_

#include <stdio.h>

#include <memory>

#include "rtc_base/buffer.h"
#include "rtc_base/constructormagic.h"
#include "rtc_base/criticalsection.h"
#include "rtc_base/messagehandler.h"
#include "rtc_base/messagequeue.h"
#include "rtc_base/third_party/sigslot/sigslot.h"

namespace rtc {

///////////////////////////////////////////////////////////////////////////////
// StreamInterface is a generic asynchronous stream interface, supporting read,
// write, and close operations, and asynchronous signalling of state changes.
// The interface is designed with file, memory, and socket implementations in
// mind.  Some implementations offer extended operations, such as seeking.
///////////////////////////////////////////////////////////////////////////////

// The following enumerations are declared outside of the StreamInterface
// class for brevity in use.

// The SS_OPENING state indicates that the stream will signal open or closed
// in the future.
enum StreamState { SS_CLOSED, SS_OPENING, SS_OPEN };

// Stream read/write methods return this value to indicate various success
// and failure conditions described below.
enum StreamResult { SR_ERROR, SR_SUCCESS, SR_BLOCK, SR_EOS };

// StreamEvents are used to asynchronously signal state transitionss.  The flags
// may be combined.
//  SE_OPEN: The stream has transitioned to the SS_OPEN state
//  SE_CLOSE: The stream has transitioned to the SS_CLOSED state
//  SE_READ: Data is available, so Read is likely to not return SR_BLOCK
//  SE_WRITE: Data can be written, so Write is likely to not return SR_BLOCK
enum StreamEvent { SE_OPEN = 1, SE_READ = 2, SE_WRITE = 4, SE_CLOSE = 8 };

class Thread;

struct StreamEventData : public MessageData {
  int events, error;
  StreamEventData(int ev, int er) : events(ev), error(er) {}
};

class StreamInterface : public MessageHandler {
 public:
  enum { MSG_POST_EVENT = 0xF1F1, MSG_MAX = MSG_POST_EVENT };

  ~StreamInterface() override;

  virtual StreamState GetState() const = 0;

  // Read attempts to fill buffer of size buffer_len.  Write attempts to send
  // data_len bytes stored in data.  The variables read and write are set only
  // on SR_SUCCESS (see below).  Likewise, error is only set on SR_ERROR.
  // Read and Write return a value indicating:
  //  SR_ERROR: an error occurred, which is returned in a non-null error
  //    argument.  Interpretation of the error requires knowledge of the
  //    stream's concrete type, which limits its usefulness.
  //  SR_SUCCESS: some number of bytes were successfully written, which is
  //    returned in a non-null read/write argument.
  //  SR_BLOCK: the stream is in non-blocking mode, and the operation would
  //    block, or the stream is in SS_OPENING state.
  //  SR_EOS: the end-of-stream has been reached, or the stream is in the
  //    SS_CLOSED state.
  virtual StreamResult Read(void* buffer,
                            size_t buffer_len,
                            size_t* read,
                            int* error) = 0;
  virtual StreamResult Write(const void* data,
                             size_t data_len,
                             size_t* written,
                             int* error) = 0;
  // Attempt to transition to the SS_CLOSED state.  SE_CLOSE will not be
  // signalled as a result of this call.
  virtual void Close() = 0;

  // Streams may signal one or more StreamEvents to indicate state changes.
  // The first argument identifies the stream on which the state change occured.
  // The second argument is a bit-wise combination of StreamEvents.
  // If SE_CLOSE is signalled, then the third argument is the associated error
  // code.  Otherwise, the value is undefined.
  // Note: Not all streams will support asynchronous event signalling.  However,
  // SS_OPENING and SR_BLOCK returned from stream member functions imply that
  // certain events will be raised in the future.
  sigslot::signal3<StreamInterface*, int, int> SignalEvent;

  // Like calling SignalEvent, but posts a message to the specified thread,
  // which will call SignalEvent.  This helps unroll the stack and prevent
  // re-entrancy.
  void PostEvent(Thread* t, int events, int err);
  // Like the aforementioned method, but posts to the current thread.
  void PostEvent(int events, int err);

  // Seek to a byte offset from the beginning of the stream.  Returns false if
  // the stream does not support seeking, or cannot seek to the specified
  // position.
  virtual bool SetPosition(size_t position);

  // Get the byte offset of the current position from the start of the stream.
  // Returns false if the position is not known.
  virtual bool GetPosition(size_t* position) const;

  // Get the byte length of the entire stream.  Returns false if the length
  // is not known.
  virtual bool GetSize(size_t* size) const;

  // Return true if flush is successful.
  virtual bool Flush();

  // Communicates the amount of data which will be written to the stream.  The
  // stream may choose to preallocate memory to accomodate this data.  The
  // stream may return false to indicate that there is not enough room (ie,
  // Write will return SR_EOS/SR_ERROR at some point).  Note that calling this
  // function should not affect the existing state of data in the stream.
  virtual bool ReserveSize(size_t size);

  //
  // CONVENIENCE METHODS
  //
  // These methods are implemented in terms of other methods, for convenience.
  //

  // Seek to the start of the stream.
  inline bool Rewind() { return SetPosition(0); }

  // WriteAll is a helper function which repeatedly calls Write until all the
  // data is written, or something other than SR_SUCCESS is returned.  Note that
  // unlike Write, the argument 'written' is always set, and may be non-zero
  // on results other than SR_SUCCESS.  The remaining arguments have the
  // same semantics as Write.
  StreamResult WriteAll(const void* data,
                        size_t data_len,
                        size_t* written,
                        int* error);

  // Similar to ReadAll.  Calls Read until buffer_len bytes have been read, or
  // until a non-SR_SUCCESS result is returned.  'read' is always set.
  StreamResult ReadAll(void* buffer,
                       size_t buffer_len,
                       size_t* read,
                       int* error);

 protected:
  StreamInterface();

  // MessageHandler Interface
  void OnMessage(Message* msg) override;

 private:
  RTC_DISALLOW_COPY_AND_ASSIGN(StreamInterface);
};

///////////////////////////////////////////////////////////////////////////////
// StreamAdapterInterface is a convenient base-class for adapting a stream.
// By default, all operations are pass-through.  Override the methods that you
// require adaptation.  Streams should really be upgraded to reference-counted.
// In the meantime, use the owned flag to indicate whether the adapter should
// own the adapted stream.
///////////////////////////////////////////////////////////////////////////////

class StreamAdapterInterface : public StreamInterface,
                               public sigslot::has_slots<> {
 public:
  explicit StreamAdapterInterface(StreamInterface* stream, bool owned = true);

  // Core Stream Interface
  StreamState GetState() const override;
  StreamResult Read(void* buffer,
                    size_t buffer_len,
                    size_t* read,
                    int* error) override;
  StreamResult Write(const void* data,
                     size_t data_len,
                     size_t* written,
                     int* error) override;
  void Close() override;

  bool SetPosition(size_t position) override;
  bool GetPosition(size_t* position) const override;
  bool GetSize(size_t* size) const override;
  bool ReserveSize(size_t size) override;
  bool Flush() override;

  void Attach(StreamInterface* stream, bool owned = true);
  StreamInterface* Detach();

 protected:
  ~StreamAdapterInterface() override;

  // Note that the adapter presents itself as the origin of the stream events,
  // since users of the adapter may not recognize the adapted object.
  virtual void OnEvent(StreamInterface* stream, int events, int err);
  StreamInterface* stream() { return stream_; }

 private:
  StreamInterface* stream_;
  bool owned_;
  RTC_DISALLOW_COPY_AND_ASSIGN(StreamAdapterInterface);
};

///////////////////////////////////////////////////////////////////////////////
// FileStream is a simple implementation of a StreamInterface, which does not
// support asynchronous notification.
///////////////////////////////////////////////////////////////////////////////

class FileStream : public StreamInterface {
 public:
  FileStream();
  ~FileStream() override;

  // The semantics of filename and mode are the same as stdio's fopen
  virtual bool Open(const std::string& filename, const char* mode, int* error);
  virtual bool OpenShare(const std::string& filename,
                         const char* mode,
                         int shflag,
                         int* error);

  // By default, reads and writes are buffered for efficiency.  Disabling
  // buffering causes writes to block until the bytes on disk are updated.
  virtual bool DisableBuffering();

  StreamState GetState() const override;
  StreamResult Read(void* buffer,
                    size_t buffer_len,
                    size_t* read,
                    int* error) override;
  StreamResult Write(const void* data,
                     size_t data_len,
                     size_t* written,
                     int* error) override;
  void Close() override;
  bool SetPosition(size_t position) override;
  bool GetPosition(size_t* position) const override;
  bool GetSize(size_t* size) const override;
  bool ReserveSize(size_t size) override;

  bool Flush() override;

 protected:
  virtual void DoClose();

  FILE* file_;

 private:
  RTC_DISALLOW_COPY_AND_ASSIGN(FileStream);
};

// FifoBuffer allows for efficient, thread-safe buffering of data between
// writer and reader. As the data can wrap around the end of the buffer,
// MemoryStreamBase can't help us here.

class FifoBuffer final : public StreamInterface {
 public:
  // Creates a FIFO buffer with the specified capacity.
  explicit FifoBuffer(size_t length);
  // Creates a FIFO buffer with the specified capacity and owner
  FifoBuffer(size_t length, Thread* owner);
  ~FifoBuffer() override;
  // Gets the amount of data currently readable from the buffer.
  bool GetBuffered(size_t* data_len) const;
  // Resizes the buffer to the specified capacity. Fails if data_length_ > size
  bool SetCapacity(size_t length);

  // Read into |buffer| with an offset from the current read position, offset
  // is specified in number of bytes.
  // This method doesn't adjust read position nor the number of available
  // bytes, user has to call ConsumeReadData() to do this.
  StreamResult ReadOffset(void* buffer,
                          size_t bytes,
                          size_t offset,
                          size_t* bytes_read);

  // Write |buffer| with an offset from the current write position, offset is
  // specified in number of bytes.
  // This method doesn't adjust the number of buffered bytes, user has to call
  // ConsumeWriteBuffer() to do this.
  StreamResult WriteOffset(const void* buffer,
                           size_t bytes,
                           size_t offset,
                           size_t* bytes_written);

  // StreamInterface methods
  StreamState GetState() const override;
  StreamResult Read(void* buffer,
                    size_t bytes,
                    size_t* bytes_read,
                    int* error) override;
  StreamResult Write(const void* buffer,
                     size_t bytes,
                     size_t* bytes_written,
                     int* error) override;
  void Close() override;
  // GetReadData returns a pointer to a buffer which is owned by the stream.
  // The buffer contains data_len bytes.  null is returned if no data is
  // available, or if the method fails.  If the caller processes the data, it
  // must call ConsumeReadData with the number of processed bytes.  GetReadData
  // does not require a matching call to ConsumeReadData if the data is not
  // processed.  Read and ConsumeReadData invalidate the buffer returned by
  // GetReadData.
  const void* GetReadData(size_t* data_len);
  void ConsumeReadData(size_t used);
  // GetWriteBuffer returns a pointer to a buffer which is owned by the stream.
  // The buffer has a capacity of buf_len bytes.  null is returned if there is
  // no buffer available, or if the method fails.  The call may write data to
  // the buffer, and then call ConsumeWriteBuffer with the number of bytes
  // written.  GetWriteBuffer does not require a matching call to
  // ConsumeWriteData if no data is written.  Write and
  // ConsumeWriteData invalidate the buffer returned by GetWriteBuffer.
  void* GetWriteBuffer(size_t* buf_len);
  void ConsumeWriteBuffer(size_t used);

  // Return the number of Write()-able bytes remaining before end-of-stream.
  // Returns false if not known.
  bool GetWriteRemaining(size_t* size) const;

 private:
  // Helper method that implements ReadOffset. Caller must acquire a lock
  // when calling this method.
  StreamResult ReadOffsetLocked(void* buffer,
                                size_t bytes,
                                size_t offset,
                                size_t* bytes_read)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(crit_);

  // Helper method that implements WriteOffset. Caller must acquire a lock
  // when calling this method.
  StreamResult WriteOffsetLocked(const void* buffer,
                                 size_t bytes,
                                 size_t offset,
                                 size_t* bytes_written)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(crit_);

  // keeps the opened/closed state of the stream
  StreamState state_ RTC_GUARDED_BY(crit_);
  // the allocated buffer
  std::unique_ptr<char[]> buffer_ RTC_GUARDED_BY(crit_);
  // size of the allocated buffer
  size_t buffer_length_ RTC_GUARDED_BY(crit_);
  // amount of readable data in the buffer
  size_t data_length_ RTC_GUARDED_BY(crit_);
  // offset to the readable data
  size_t read_position_ RTC_GUARDED_BY(crit_);
  // stream callbacks are dispatched on this thread
  Thread* owner_;
  // object lock
  CriticalSection crit_;
  RTC_DISALLOW_COPY_AND_ASSIGN(FifoBuffer);
};

}  // namespace rtc

#endif  // RTC_BASE_STREAM_H_
