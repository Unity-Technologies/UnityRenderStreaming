// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_STREAMS_READABLE_STREAM_OPERATIONS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_STREAMS_READABLE_STREAM_OPERATIONS_H_

#include "base/optional.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise.h"
#include "third_party/blink/renderer/bindings/core/v8/script_value.h"
#include "third_party/blink/renderer/core/core_export.h"

namespace blink {

class UnderlyingSourceBase;
class ExceptionState;
class MessagePort;
class ScriptState;

// This class has various methods for ReadableStream[Reader] implemented with
// V8 Extras.
// All methods should be called in an appropriate V8 context. All ScriptValue
// arguments must not be empty.
//
// Boolean methods return an optional bool, where an empty value indicates that
// Javascript failed to return a value (ie. an exception occurred). Exceptions
// are not caught, so that they can be handled by user Javascript. This implicit
// exception passing is error-prone and bad.
//
// In methods which take an ExceptionState& parameter, exception passing is
// explicit. Callers must check exception_state.HadException() on
// return. Although these methods return base::nullopt if and only if an
// exception was thrown, outside of unit tests exception_state should always be
// used to determine failure.
//
// TODO(ricea): Add ExceptionState arguments to the rest of the non-Promise
// methods to make exception passing explicit. https://crbug.com/853189.
class CORE_EXPORT ReadableStreamOperations {
  STATIC_ONLY(ReadableStreamOperations);

 public:
  // createReadableStreamWithExternalController
  // Instantiates ReadableStream defined in the script and returns it.
  // If the caller supplies an invalid strategy (e.g. one that returns
  // negative sizes, or doesn't have appropriate properties), or an exception
  // occurs for another reason, this will return an empty value.
  static ScriptValue CreateReadableStream(ScriptState*,
                                          UnderlyingSourceBase*,
                                          ScriptValue strategy);

  // createReadableStream
  // Instantiates ReadableStream defined in the script and returns it.
  static ScriptValue CreateReadableStream(ScriptState*,
                                          ScriptValue underlying_source,
                                          ScriptValue strategy,
                                          ExceptionState& exception_state);

  // createBuiltInCountQueuingStrategy
  // If the constructor throws, this will return an empty value.
  static ScriptValue CreateCountQueuingStrategy(ScriptState*,
                                                size_t high_water_mark);

  // AcquireReadableStreamDefaultReader
  // This function assumes |IsReadableStream(stream)|.
  static ScriptValue GetReader(ScriptState*,
                               ScriptValue stream,
                               ExceptionState&);

  // IsReadableStream, exception-catching version. Exceptions will be passed to
  // |exception_state|.
  static base::Optional<bool> IsReadableStream(ScriptState*,
                                               ScriptValue,
                                               ExceptionState& exception_state);

  // Performs IsReadableStream.
  // Catches exceptions, and returns false if there are any. Should only be used
  // in a DCHECK statement that passes when the return value is true.
  static bool IsReadableStreamForDCheck(ScriptState*, ScriptValue);

  // IsReadableStreamDisturbed.
  // This function assumes |IsReadableStream(stream)|.
  static base::Optional<bool> IsDisturbed(ScriptState*,
                                          ScriptValue stream,
                                          ExceptionState& exception_state);

  // Performs IsReadableStreamDisturbed.
  // Catches exceptions, and returns false if there are any. Should only be used
  // in a DCHECK statement that passes when the return value is false.
  static bool IsDisturbedForDCheck(ScriptState*, ScriptValue stream);

  // IsReadableStreamLocked.
  // This function assumes |IsReadableStream(stream)|.
  static base::Optional<bool> IsLocked(ScriptState*,
                                       ScriptValue stream,
                                       ExceptionState&);

  // Performs IsReadableStreamLocked.
  // Catches exceptions, and returns false if there are any. Should only be used
  // in a DCHECK statement that passes when the return value is false.
  static bool IsLockedForDCheck(ScriptState*, ScriptValue stream);

  // IsReadableStreamReadable.
  // This function assumes |IsReadableStream(stream)|.
  static base::Optional<bool> IsReadable(ScriptState*,
                                         ScriptValue stream,
                                         ExceptionState& exception_state);

  // IsReadableStreamClosed.
  // This function assumes |IsReadableStream(stream)|.
  static base::Optional<bool> IsClosed(ScriptState*,
                                       ScriptValue stream,
                                       ExceptionState& exception_state);

  // IsReadableStreamErrored.
  // This function assumes |IsReadableStream(stream)|.
  static base::Optional<bool> IsErrored(ScriptState*,
                                        ScriptValue stream,
                                        ExceptionState& exception_state);

  // IsReadableStreamDefaultReader.
  static base::Optional<bool> IsReadableStreamDefaultReader(
      ScriptState*,
      ScriptValue,
      ExceptionState& exception_state);

  // ReadableStreamDefaultReaderRead
  // This function assumes |IsReadableStreamDefaultReader(reader)|.
  // If an exception occurs, returns a rejected promise.
  static ScriptPromise DefaultReaderRead(ScriptState*, ScriptValue reader);

  // ReadableStreamTee
  // This function assumes |IsReadableStream(stream)| and |!IsLocked(stream)|
  static ScriptValue Tee(ScriptState*, ScriptValue stream, ExceptionState&);

  // ReadableStreamSerialize. The MessagePort passed in must be one half of a
  // MessageChannel. The other half can later be passed to Deserialize to
  // produce an equivalent ReadableStream in a different context.
  static void Serialize(ScriptState*,
                        ScriptValue stream,
                        MessagePort* port,
                        ExceptionState&);

  // ReadableStreamDeserialize returns a new ReadableStream in the current
  // context given a MessagePort which is bound to one which was previously
  // passed to Serialize().
  static ScriptValue Deserialize(ScriptState*, MessagePort*, ExceptionState&);

  // ReadableStreamCancel
  // This function assumes |IsReadableStream(stream)|
  static ScriptPromise Cancel(ScriptState*,
                              ScriptValue stream,
                              ScriptValue reason,
                              ExceptionState& exception_state);

  // ReadableStreamPipeTo
  // This function assumes |IsReadableStream(stream)|, |!IsLocked(stream)|,
  // |IsWritableStream(destination)| and |!IsLocked(destination)|.
  static ScriptPromise PipeTo(ScriptState*,
                              ScriptValue stream,
                              ScriptValue destination,
                              ScriptValue options,
                              ExceptionState& exception_state);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_STREAMS_READABLE_STREAM_OPERATIONS_H_
