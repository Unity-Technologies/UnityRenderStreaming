/*
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_PATH_BYTE_STREAM_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_PATH_BYTE_STREAM_H_

#include <memory>

#include "base/memory/ptr_util.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

template <typename DataType>
union ByteType {
  DataType value;
  unsigned char bytes[sizeof(DataType)];
};

class SVGPathByteStream {
  USING_FAST_MALLOC(SVGPathByteStream);

 public:
  static std::unique_ptr<SVGPathByteStream> Create() {
    return base::WrapUnique(new SVGPathByteStream);
  }

  std::unique_ptr<SVGPathByteStream> Clone() const {
    return base::WrapUnique(new SVGPathByteStream(data_));
  }

  typedef Vector<unsigned char> Data;
  typedef Data::const_iterator DataIterator;

  DataIterator begin() const { return data_.begin(); }
  DataIterator end() const { return data_.end(); }
  void Append(const unsigned char* data, wtf_size_t data_size) {
    data_.Append(data, data_size);
  }
  void clear() { data_.clear(); }
  void ReserveInitialCapacity(wtf_size_t size) {
    data_.ReserveInitialCapacity(size);
  }
  void ShrinkToFit() { data_.ShrinkToFit(); }
  bool IsEmpty() const { return data_.IsEmpty(); }
  unsigned size() const { return data_.size(); }

  bool operator==(const SVGPathByteStream& other) const {
    return data_ == other.data_;
  }

 private:
  SVGPathByteStream() = default;
  SVGPathByteStream(const Data& data) : data_(data) {}

  Data data_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_SVG_SVG_PATH_BYTE_STREAM_H_
