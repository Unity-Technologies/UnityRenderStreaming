/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_BLOB_BLOB_DATA_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_BLOB_BLOB_DATA_H_

#include <memory>
#include "base/gtest_prod_util.h"
#include "base/thread_annotations.h"
#include "third_party/blink/public/mojom/blob/blob.mojom-blink.h"
#include "third_party/blink/public/mojom/blob/data_element.mojom-blink.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/platform/wtf/thread_safe_ref_counted.h"
#include "third_party/blink/renderer/platform/wtf/threading_primitives.h"

namespace blink {
namespace mojom {
namespace blink {
class BlobRegistry;
}
}  // namespace mojom

class BlobBytesProvider;
class BlobDataHandle;

class PLATFORM_EXPORT RawData : public ThreadSafeRefCounted<RawData> {
 public:
  static scoped_refptr<RawData> Create() {
    return base::AdoptRef(new RawData());
  }

  const char* data() const { return data_.data(); }
  size_t length() const { return data_.size(); }
  Vector<char>* MutableData() { return &data_; }

 private:
  RawData();

  Vector<char> data_;
};

class PLATFORM_EXPORT BlobData {
  USING_FAST_MALLOC(BlobData);

 public:
  static constexpr long long kToEndOfFile = -1;

  static std::unique_ptr<BlobData> Create();

  // Calling append* on objects returned by createFor___WithUnknownSize will
  // check-fail. The caller can only have an unknown-length file if it is the
  // only item in the blob.
  static std::unique_ptr<BlobData> CreateForFileWithUnknownSize(
      const String& path);
  static std::unique_ptr<BlobData> CreateForFileWithUnknownSize(
      const String& path,
      double expected_modification_time);
  static std::unique_ptr<BlobData> CreateForFileSystemURLWithUnknownSize(
      const KURL& file_system_url,
      double expected_modification_time);

  // Detaches from current thread so that it can be passed to another thread.
  void DetachFromCurrentThread();

  const String& ContentType() const { return content_type_; }
  void SetContentType(const String&);

  const Vector<mojom::blink::DataElementPtr>& Elements() const {
    return elements_;
  }
  Vector<mojom::blink::DataElementPtr> ReleaseElements() {
    return std::move(elements_);
  }

  void AppendBytes(const void*, size_t length);
  void AppendData(scoped_refptr<RawData>);
  void AppendFile(const String& path,
                  long long offset,
                  long long length,
                  double expected_modification_time);

  // The given blob must not be a file with unknown size. Please use the
  // File::appendTo instead.
  void AppendBlob(scoped_refptr<BlobDataHandle>,
                  long long offset,
                  long long length);
  void AppendFileSystemURL(const KURL&,
                           long long offset,
                           long long length,
                           double expected_modification_time);
  void AppendText(const String&, bool normalize_line_endings_to_native);

  // The value of the size property for a Blob who has this data.
  // BlobDataItem::toEndOfFile if the Blob has a file whose size was not yet
  // determined.
  uint64_t length() const;

  bool IsSingleUnknownSizeFile() const {
    return file_composition_ == FileCompositionStatus::SINGLE_UNKNOWN_SIZE_FILE;
  }

 private:
  enum class FileCompositionStatus {
    SINGLE_UNKNOWN_SIZE_FILE,
    NO_UNKNOWN_SIZE_FILES
  };

  explicit BlobData(FileCompositionStatus composition)
      : file_composition_(composition) {}

  void AppendDataInternal(base::span<const char> data,
                          scoped_refptr<RawData> = nullptr);

  String content_type_;
  FileCompositionStatus file_composition_;

  Vector<mojom::blink::DataElementPtr> elements_;
  size_t current_memory_population_ = 0;
  BlobBytesProvider* last_bytes_provider_ = nullptr;

  DISALLOW_COPY_AND_ASSIGN(BlobData);
};

class PLATFORM_EXPORT BlobDataHandle
    : public ThreadSafeRefCounted<BlobDataHandle> {
 public:
  // For empty blob construction.
  static scoped_refptr<BlobDataHandle> Create() {
    return base::AdoptRef(new BlobDataHandle());
  }

  // For initial creation.
  static scoped_refptr<BlobDataHandle> Create(std::unique_ptr<BlobData> data,
                                              long long size) {
    return base::AdoptRef(new BlobDataHandle(std::move(data), size));
  }

  // For deserialization of script values and ipc messages.
  static scoped_refptr<BlobDataHandle> Create(const String& uuid,
                                              const String& type,
                                              long long size) {
    return base::AdoptRef(new BlobDataHandle(uuid, type, size));
  }

  static scoped_refptr<BlobDataHandle> Create(const String& uuid,
                                       const String& type,
                                       long long size,
                                       mojom::blink::BlobPtrInfo blob_info) {
    if (blob_info.is_valid()) {
      return base::AdoptRef(
          new BlobDataHandle(uuid, type, size, std::move(blob_info)));
    }
    return base::AdoptRef(new BlobDataHandle(uuid, type, size));
  }

  String Uuid() const { return uuid_.IsolatedCopy(); }
  String GetType() const { return type_.IsolatedCopy(); }
  unsigned long long size() const { return size_; }

  bool IsSingleUnknownSizeFile() const { return is_single_unknown_size_file_; }

  ~BlobDataHandle();

  mojom::blink::BlobPtr CloneBlobPtr();
  network::mojom::blink::DataPipeGetterPtr AsDataPipeGetter();

  void ReadAll(mojo::ScopedDataPipeProducerHandle,
               mojom::blink::BlobReaderClientPtr);
  void ReadRange(uint64_t offset,
                 uint64_t length,
                 mojo::ScopedDataPipeProducerHandle,
                 mojom::blink::BlobReaderClientPtr);

  static mojom::blink::BlobRegistry* GetBlobRegistry();
  static void SetBlobRegistryForTesting(mojom::blink::BlobRegistry*);

 private:
  BlobDataHandle();
  BlobDataHandle(std::unique_ptr<BlobData>, long long size);
  BlobDataHandle(const String& uuid, const String& type, long long size);
  BlobDataHandle(const String& uuid,
                 const String& type,
                 long long size,
                 mojom::blink::BlobPtrInfo);

  const String uuid_;
  const String type_;
  const long long size_;
  const bool is_single_unknown_size_file_;
  // This class is supposed to be thread safe. So to be able to use the mojo
  // Blob interface from multiple threads store a InterfacePtrInfo combined with
  // a mutex, and make sure any access to the mojo interface is done protected
  // by the mutex.
  mojom::blink::BlobPtrInfo blob_info_ GUARDED_BY(blob_info_mutex_);
  Mutex blob_info_mutex_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_BLOB_BLOB_DATA_H_
