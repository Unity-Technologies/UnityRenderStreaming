// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TEST_FUZZED_DATA_PROVIDER_H_
#define BASE_TEST_FUZZED_DATA_PROVIDER_H_

#include <stddef.h>
#include <stdint.h>

#include <algorithm>
#include <limits>
#include <string>
#include <utility>
#include <vector>

// A single-header library providing an utility class to break up an array of
// bytes (supposedly provided by a fuzzing engine) for multiple consumers.
// Whenever run on the same input, provides the same output, as long as its
// methods are called in the same order, with the same arguments.

// TODO(mmoroz): move this out of //base as it should be an independent library.
namespace base {

class FuzzedDataProvider {
 public:
  typedef uint8_t data_type;

  // |data| is an array of length |size| that the FuzzedDataProvider wraps to
  // provide more granular access. |data| must outlive the FuzzedDataProvider.
  FuzzedDataProvider(const uint8_t* data, size_t size)
      : data_ptr_(data), remaining_bytes_(size) {}
  ~FuzzedDataProvider() = default;

  // Returns a std::vector containing |num_bytes| of input data. If fewer than
  // |num_bytes| of data remain, returns a shorter std::vector containing all
  // of the data that's left.
  template <typename T = data_type>
  std::vector<T> ConsumeBytes(size_t num_bytes) {
    static_assert(sizeof(T) == sizeof(data_type), "Incompatible data type.");

    num_bytes = std::min(num_bytes, remaining_bytes_);

    // The point of using the size-based constructor below is to increase the
    // odds of having a vector object with capacity being equal to the length.
    // That part is always implementation specific, but at least both libc++ and
    // libstdc++ allocate the requested number of bytes in that constructor,
    // which seems to be a natual choice for other implementations as well.
    // To increase the odds even more, we also call |shrink_to_fit| below.
    std::vector<T> result(num_bytes);
    std::memcpy(result.data(), data_ptr_, num_bytes);
    Advance(num_bytes);

    // Even though |shrink_to_fit| is also implementation specific, we expect it
    // to provide an additional assurance in case vector's constructor allocated
    // a buffer which is larger than the actual amount of data we put inside it.
    result.shrink_to_fit();
    return result;
  }

  // Prefer using |ConsumeBytes| unless you actually need a std::string object.
  // Returns a std::string containing |num_bytes| of input data. If fewer than
  // |num_bytes| of data remain, returns a shorter std::string containing all
  // of the data that's left.
  std::string ConsumeBytesAsString(size_t num_bytes) {
    static_assert(sizeof(std::string::value_type) == sizeof(data_type),
                  "ConsumeBytesAsString cannot convert the data to a string.");

    num_bytes = std::min(num_bytes, remaining_bytes_);
    std::string result(
        reinterpret_cast<const std::string::value_type*>(data_ptr_), num_bytes);
    Advance(num_bytes);
    return result;
  }

  // Returns a number in the range [min, max] by consuming bytes from the input
  // data. The value might not be uniformly distributed in the given range. If
  // there's no input data left, always returns |min|. |min| must be less than
  // or equal to |max|.
  template <typename T>
  T ConsumeIntegralInRange(T min, T max) {
    static_assert(std::is_integral<T>::value, "An integral type is required.");
    static_assert(sizeof(T) <= sizeof(uint64_t), "Unsupported integral type.");

    if (min > max)
      abort();

    // Use the biggest type possible to hold the range and the result.
    uint64_t range = max - min;
    uint64_t result = 0;
    size_t offset = 0;

    while (offset < sizeof(T) * CHAR_BIT && (range >> offset) > 0 &&
           remaining_bytes_ != 0) {
      // Pull bytes off the end of the seed data. Experimentally, this seems to
      // allow the fuzzer to more easily explore the input space. This makes
      // sense, since it works by modifying inputs that caused new code to run,
      // and this data is often used to encode length of data read by
      // |ConsumeBytes|. Separating out read lengths makes it easier modify the
      // contents of the data that is actually read.
      --remaining_bytes_;
      result = (result << CHAR_BIT) | data_ptr_[remaining_bytes_];
      offset += CHAR_BIT;
    }

    // Avoid division by 0, in the case |range + 1| results in overflow.
    if (range != std::numeric_limits<decltype(range)>::max())
      result = result % (range + 1);

    return min + static_cast<T>(result);
  }

  // Returns a std::string of length from 0 to |max_length|. When it runs out of
  // input data, returns what remains of the input. Designed to be more stable
  // with respect to a fuzzer inserting characters than just picking a random
  // length and then consuming that many bytes with |ConsumeBytes|.
  std::string ConsumeRandomLengthString(size_t max_length) {
    // Reads bytes from the start of |data_ptr_|. Maps "\\" to "\", and maps "\"
    // followed by anything else to the end of the string. As a result of this
    // logic, a fuzzer can insert characters into the string, and the string
    // will be lengthened to include those new characters, resulting in a more
    // stable fuzzer than picking the length of a string independently from
    // picking its contents.
    std::string result;
    for (size_t i = 0; i < max_length && remaining_bytes_ != 0; ++i) {
      char next = static_cast<char>(data_ptr_[0]);
      Advance(1);
      if (next == '\\' && remaining_bytes_ != 0) {
        next = static_cast<char>(data_ptr_[0]);
        Advance(1);
        if (next != '\\')
          return result;
      }
      result += next;
    }

    result.shrink_to_fit();
    return result;
  }

  // Returns a std::vector containing all remaining bytes of the input data.
  template <typename T = data_type>
  std::vector<T> ConsumeRemainingBytes() {
    return ConsumeBytes<T>(remaining_bytes_);
  }

  // Prefer using |ConsumeRemainingBytes| unless you actually need a std::string
  // object.
  // Returns a std::vector containing all remaining bytes of the input data.
  std::string ConsumeRemainingBytesAsString() {
    return ConsumeBytesAsString(remaining_bytes_);
  }

  // TODO(mmoroz): consider deprecating these methods.
  uint32_t ConsumeUint32InRange(uint32_t min, uint32_t max) {
    return ConsumeIntegralInRange(min, max);
  }

  int32_t ConsumeInt32InRange(int32_t min, int32_t max) {
    return ConsumeIntegralInRange(min, max);
  }

  int ConsumeIntInRange(int min, int max) {
    return ConsumeIntegralInRange(min, max);
  }

  // Reads one byte and returns a bool, or false when no data remains.
  bool ConsumeBool() { return 1 & ConsumeUint8(); }

  // Returns a uint8_t from the input or 0 if nothing remains. This is
  // equivalent to ConsumeUint32InRange(0, 0xFF).
  uint8_t ConsumeUint8() {
    return ConsumeIntegralInRange(std::numeric_limits<uint8_t>::min(),
                                  std::numeric_limits<uint8_t>::max());
  }

  // Returns a uint16_t from the input. If fewer than 2 bytes of data remain
  // will fill the most significant bytes with 0. This is equivalent to
  // ConsumeUint32InRange(0, 0xFFFF).
  uint16_t ConsumeUint16() {
    return ConsumeIntegralInRange(std::numeric_limits<uint16_t>::min(),
                                  std::numeric_limits<uint16_t>::max());
  }

  // Returns a value from |array|, consuming as many bytes as needed to do so.
  // |array| must be a fixed-size array.
  template <typename Type, size_t size>
  Type PickValueInArray(Type (&array)[size]) {
    return array[ConsumeIntegralInRange<size_t>(0, size - 1)];
  }

  // Reports the remaining bytes available for fuzzed input.
  size_t remaining_bytes() { return remaining_bytes_; }

 private:
  FuzzedDataProvider(const FuzzedDataProvider&) = delete;
  FuzzedDataProvider& operator=(const FuzzedDataProvider&) = delete;

  void Advance(size_t num_bytes) {
    if (num_bytes > remaining_bytes_)
      abort();

    data_ptr_ += num_bytes;
    remaining_bytes_ -= num_bytes;
  }

  const data_type* data_ptr_;
  size_t remaining_bytes_;
};

}  // namespace base

#endif  // BASE_TEST_FUZZED_DATA_PROVIDER_H_
