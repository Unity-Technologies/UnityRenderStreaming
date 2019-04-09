// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_PARKABLE_STRING_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_PARKABLE_STRING_H_

#include <map>
#include <memory>
#include <set>
#include <utility>

#include "base/gtest_prod_util.h"
#include "base/macros.h"
#include "base/memory/scoped_refptr.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/ref_counted.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/platform/wtf/threading.h"
#include "third_party/blink/renderer/platform/wtf/threading_primitives.h"

// ParkableString represents a string that may be parked in memory, that it its
// underlying memory address may change. Its content can be retrieved with the
// |ToString()| method.
// As a consequence, the inner pointer should never be cached, and only touched
// through a string returned by the |ToString()| method.
//
// As with WTF::AtomicString, this class is *not* thread-safe, and strings
// created on a thread must always be used on the same thread.

namespace blink {

struct CompressionTaskParams;

// A parked string is parked by calling |Park()|, and unparked by calling
// |ToString()| on a parked string.
// |Lock()| does *not* unpark a string, and |ToString()| must be called on
// a single thread, the one on which the string was created. Only |Lock()|
// and |Unlock()| can be called from any thread.
class PLATFORM_EXPORT ParkableStringImpl final
    : public RefCounted<ParkableStringImpl> {
 public:
  // Histogram buckets, exported for testing.
  enum class ParkingAction {
    kParkedInBackground = 0,
    kUnparkedInBackground = 1,
    kUnparkedInForeground = 2,
    kMaxValue = kUnparkedInForeground
  };

  enum class ParkableState { kParkable, kNotParkable };
  enum class ParkingMode { kIfCompressedDataExists, kAlways };

  // Not all parkable strings can actually be parked. If |parkable| is
  // kNotParkable, then one cannot call |Park()|, and the underlying StringImpl
  // will not move.
  ParkableStringImpl(scoped_refptr<StringImpl>&& impl, ParkableState parkable);
  ~ParkableStringImpl();

  void Lock();
  void Unlock();

  // The returned string may be used as a normal one, as long as the
  // returned value (or a copy of it) is alive.
  const String& ToString();

  // See the matching String methods.
  bool is_8bit() const { return is_8bit_; }
  unsigned length() const { return length_; }
  unsigned CharactersSizeInBytes() const;

  // A parked string cannot be accessed until it has been |Unpark()|-ed.
  //
  // Parking may be synchronous, and will be if compressed data is already
  // available. If |mode| is |kIfCompressedDataExists|, then parking will always
  // be synchronous.
  //
  // Returns true if the string is being parked or has been parked.
  bool Park(ParkingMode mode);
  // Returns true iff the string can be parked. This does not mean that the
  // string can be parked now, merely that it is eligible to be parked at some
  // point.
  bool may_be_parked() const { return may_be_parked_; }
  // Returns true if the string is parked.
  bool is_parked() const;
  // Returns whether synchronous parking is possible, that is the string was
  // parked in the past.
  bool has_compressed_data() const { return !!compressed_; }
  // Returns the compressed size, must not be called unless the string has a
  // compressed representation.
  size_t compressed_size() const {
    DCHECK(has_compressed_data());
    return compressed_->size();
  }

 private:
  enum class State;

  // Whether the string can be parked now. Must be called with |mutex_| held,
  // and the return value is valid as long as the mutex is held.
  bool CanParkNow() const;
  void Unpark();
  // Called on the main thread after compression is done.
  // |params| is the same as the one passed to |CompressInBackground()|,
  // |compressed| is the compressed data, nullptr if compression failed.
  void OnParkingCompleteOnMainThread(
      std::unique_ptr<CompressionTaskParams> params,
      std::unique_ptr<Vector<uint8_t>> compressed);

  // Background thread.
  static void CompressInBackground(std::unique_ptr<CompressionTaskParams>);

  Mutex mutex_;  // protects lock_depth_.
  int lock_depth_;

  // Main thread only.
  State state_;
  String string_;
  std::unique_ptr<Vector<uint8_t>> compressed_;

  const bool may_be_parked_;
  const bool is_8bit_;
  const unsigned length_;

#if DCHECK_IS_ON()
  const ThreadIdentifier owning_thread_;
#endif

  void AssertOnValidThread() const {
#if DCHECK_IS_ON()
    DCHECK_EQ(owning_thread_, CurrentThread());
#endif
  }

  FRIEND_TEST_ALL_PREFIXES(ParkableStringTest, LockUnlock);
  FRIEND_TEST_ALL_PREFIXES(ParkableStringTest, LockParkedString);
  DISALLOW_COPY_AND_ASSIGN(ParkableStringImpl);
};

class PLATFORM_EXPORT ParkableString final {
 public:
  ParkableString() : impl_(nullptr) {}
  explicit ParkableString(scoped_refptr<StringImpl>&& impl);
  ParkableString(const ParkableString& rhs) : impl_(rhs.impl_) {}
  ~ParkableString();

  // Locks a string. A string is unlocked when the number of Lock()/Unlock()
  // calls match. A locked string cannot be parked.
  // Can be called from any thread.
  void Lock() const;

  // Unlocks a string.
  // Can be called from any thread.
  void Unlock() const;

  // See the matching String methods.
  bool Is8Bit() const;
  bool IsNull() const { return !impl_; }
  unsigned length() const { return impl_ ? impl_->length() : 0; }
  bool may_be_parked() const { return impl_ && impl_->may_be_parked(); }

  ParkableStringImpl* Impl() const { return impl_ ? impl_.get() : nullptr; }
  // Returns an unparked version of the string.
  // The string is guaranteed to be valid for
  // max(lifetime of a copy of the returned reference, current thread task).
  String ToString() const;
  wtf_size_t CharactersSizeInBytes() const;

  // Causes the string to be unparked. Note that the pointer must not be
  // cached.
  const LChar* Characters8() const { return ToString().Characters8(); }
  const UChar* Characters16() const { return ToString().Characters16(); }

 private:
  scoped_refptr<ParkableStringImpl> impl_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_PARKABLE_STRING_H_
