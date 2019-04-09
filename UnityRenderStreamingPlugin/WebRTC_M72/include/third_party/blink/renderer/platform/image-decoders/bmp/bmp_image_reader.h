/*
 * Copyright (c) 2008, 2009, Google Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_IMAGE_DECODERS_BMP_BMP_IMAGE_READER_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_IMAGE_DECODERS_BMP_BMP_IMAGE_READER_H_

#include <stdint.h>
#include "third_party/blink/renderer/platform/image-decoders/fast_shared_buffer_reader.h"
#include "third_party/blink/renderer/platform/image-decoders/image_decoder.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/cpu.h"
#include "third_party/blink/renderer/platform/wtf/noncopyable.h"

namespace blink {

// This class decodes a BMP image.  It is used in the BMP and ICO decoders,
// which wrap it in the appropriate code to read file headers, etc.
class PLATFORM_EXPORT BMPImageReader final {
  USING_FAST_MALLOC(BMPImageReader);
  WTF_MAKE_NONCOPYABLE(BMPImageReader);

 public:
  // Read a value from |buffer|, converting to an int assuming little
  // endianness
  static inline uint16_t ReadUint16(const char* buffer) {
    return *reinterpret_cast<const uint16_t*>(buffer);
  }

  static inline uint32_t ReadUint32(const char* buffer) {
    return *reinterpret_cast<const uint32_t*>(buffer);
  }

  // |parent| is the decoder that owns us.
  // |start_offset| points to the start of the BMP within the file.
  // |buffer| points at an empty ImageFrame that we'll initialize and
  // fill with decoded data.
  BMPImageReader(ImageDecoder* parent,
                 size_t decoded_and_header_offset,
                 size_t img_data_offset,
                 bool is_in_ico);

  void SetBuffer(ImageFrame* buffer) { buffer_ = buffer; }
  void SetData(SegmentReader* data) {
    data_ = data;
    fast_reader_.SetData(data);
  }

  // Does the actual decoding.  If |only_size| is true, decoding only
  // progresses as far as necessary to get the image size.  Returns
  // whether decoding succeeded.
  bool DecodeBMP(bool only_size);

 private:
  friend class PixelChangedScoper;

  // Helper for DecodeBMP() which will call either ProcessRLEData() or
  // ProcessNonRLEData(), depending on the value of |non_rle|, call any
  // appropriate notifications to deal with the result, then return whether
  // decoding succeeded.
  bool DecodePixelData(bool non_rle);

  // The various BMP compression types.  We don't currently decode all
  // these.
  enum CompressionType {
    // Universal types
    RGB = 0,
    RLE8 = 1,
    RLE4 = 2,
    // Windows V3+ only
    BITFIELDS = 3,
    JPEG = 4,
    PNG = 5,
    // OS/2 2.x-only
    HUFFMAN1D,  // Stored in file as 3
    RLE24,      // Stored in file as 4
  };
  enum ProcessingResult {
    kSuccess,
    kFailure,
    kInsufficientData,
  };

  // These are based on the Windows BITMAPINFOHEADER and RGBTRIPLE
  // structs, but with unnecessary entries removed.
  struct BitmapInfoHeader {
    DISALLOW_NEW();
    uint32_t bi_size;
    int32_t bi_width;
    int32_t bi_height;
    uint16_t bi_bit_count;
    CompressionType bi_compression;
    uint32_t bi_clr_used;
  };
  struct RGBTriple {
    DISALLOW_NEW();
    uint8_t rgb_blue;
    uint8_t rgb_green;
    uint8_t rgb_red;
  };

  inline uint8_t ReadUint8(size_t offset) const {
    return fast_reader_.GetOneByte(decoded_offset_ + offset);
  }

  inline uint16_t ReadUint16(int offset) const {
    char buffer[2];
    const char* data =
        fast_reader_.GetConsecutiveData(decoded_offset_ + offset, 2, buffer);
    return ReadUint16(data);
  }

  inline uint32_t ReadUint32(int offset) const {
    char buffer[4];
    const char* data =
        fast_reader_.GetConsecutiveData(decoded_offset_ + offset, 4, buffer);
    return ReadUint32(data);
  }

  // Determines the size of the BMP info header.  Returns true if the size
  // is valid.
  bool ReadInfoHeaderSize();

  // Processes the BMP info header.  Returns true if the info header could
  // be decoded.
  bool ProcessInfoHeader();

  // Helper function for ProcessInfoHeader() which does the actual reading
  // of header values from the byte stream.  Returns false on error.
  bool ReadInfoHeader();

  // Returns true if this is a Windows V4+ BMP.
  inline bool IsWindowsV4Plus() const {
    // Windows V4 info header is 108 bytes.  V5 is 124 bytes.
    return (info_header_.bi_size == 108) || (info_header_.bi_size == 124);
  }

  // Returns false if consistency errors are found in the info header.
  bool IsInfoHeaderValid() const;

  // For BI_BITFIELDS images, initializes the bit_masks_[] and
  // bit_offsets_[] arrays.  ProcessInfoHeader() will initialize these for
  // other compression types where needed.
  bool ProcessBitmasks();

  // For paletted images, allocates and initializes the color_table_[]
  // array.
  bool ProcessColorTable();

  // The next two functions return a ProcessingResult instead of a bool so
  // they can avoid calling parent_->SetFailed(), which could lead to memory
  // corruption since that will delete |this| but some callers still want
  // to access member variables after they return.

  // Processes an RLE-encoded image.
  ProcessingResult ProcessRLEData();

  // Processes a set of non-RLE-compressed pixels.  Two cases:
  //   * in_rle = true: the data is inside an RLE-encoded bitmap.  Tries to
  //     process |num_pixels| pixels on the current row.
  //   * in_rle = false: the data is inside a non-RLE-encoded bitmap.
  //     |num_pixels| is ignored.  Expects |coord_| to point at the
  //     beginning of the next row to be decoded.  Tries to process as
  //     many complete rows as possible.  Returns InsufficientData if
  //     there wasn't enough data to decode the whole image.
  ProcessingResult ProcessNonRLEData(bool in_rle, int num_pixels);

  // Returns true if the current y-coordinate plus |num_rows| would be past
  // the end of the image.  Here "plus" means "toward the end of the
  // image", so downwards for is_top_down_ images and upwards otherwise.
  inline bool PastEndOfImage(int num_rows) {
    return is_top_down_ ? ((coord_.Y() + num_rows) >= parent_->Size().Height())
                        : ((coord_.Y() - num_rows) < 0);
  }

  // Returns the pixel data for the current |decoded_offset_| in a uint32_t.
  // NOTE: Only as many bytes of the return value as are needed to hold
  // the pixel data will actually be set.
  inline uint32_t ReadCurrentPixel(int bytes_per_pixel) const {
    // We need at most 4 bytes, starting at decoded_offset_.
    char buffer[4];
    const char* encoded_pixel = fast_reader_.GetConsecutiveData(
        decoded_offset_, bytes_per_pixel, buffer);
    switch (bytes_per_pixel) {
      case 2:
        return ReadUint16(encoded_pixel);

      case 3: {
        // It doesn't matter that we never set the most significant byte
        // of the return value, the caller won't read it.
        uint32_t pixel;
        memcpy(&pixel, encoded_pixel, 3);
        return pixel;
      }

      case 4:
        return ReadUint32(encoded_pixel);

      default:
        NOTREACHED();
        return 0;
    }
  }

  // Returns the value of the desired component (0, 1, 2, 3 == R, G, B, A)
  // in the given pixel data.
  inline unsigned GetComponent(uint32_t pixel, int component) const {
    uint8_t value =
        (pixel & bit_masks_[component]) >> bit_shifts_right_[component];
    return lookup_table_addresses_[component]
               ? lookup_table_addresses_[component][value]
               : value;
  }

  inline unsigned GetAlpha(uint32_t pixel) const {
    // For images without alpha, return alpha of 0xff.
    return bit_masks_[3] ? GetComponent(pixel, 3) : 0xff;
  }

  // Sets the current pixel to the color given by |color_index|.  This also
  // increments the relevant local variables to move the current pixel
  // right by one.
  inline void SetI(size_t color_index) {
    SetRGBA(color_table_[color_index].rgb_red,
            color_table_[color_index].rgb_green,
            color_table_[color_index].rgb_blue, 0xff);
  }

  // Like SetI(), but with the individual component values specified.
  inline void SetRGBA(unsigned red,
                      unsigned green,
                      unsigned blue,
                      unsigned alpha) {
    buffer_->SetRGBA(coord_.X(), coord_.Y(), red, green, blue, alpha);
    coord_.Move(1, 0);
  }

  // Fills pixels from the current X-coordinate up to, but not including,
  // |end_coord| with the color given by the individual components.  This
  // also increments the relevant local variables to move the current
  // pixel right to |end_coord|.
  inline void FillRGBA(int end_coord,
                       unsigned red,
                       unsigned green,
                       unsigned blue,
                       unsigned alpha) {
    while (coord_.X() < end_coord)
      SetRGBA(red, green, blue, alpha);
  }

  // Resets the relevant local variables to start drawing at the left edge
  // of the "next" row, where "next" is above or below the current row
  // depending on the value of |is_top_down_|.
  void MoveBufferToNextRow();

  // The decoder that owns us.
  ImageDecoder* parent_;

  // The destination for the pixel data.
  ImageFrame* buffer_;

  // The file to decode.
  scoped_refptr<SegmentReader> data_;
  FastSharedBufferReader fast_reader_;

  // An index into |data_| representing how much we've already decoded.
  size_t decoded_offset_;

  // The file offset at which the BMP info header starts.
  size_t header_offset_;

  // The file offset at which the actual image bits start.  When decoding
  // ICO files, this is set to 0, since it's not stored anywhere in a
  // header; the reader functions expect the image data to start
  // immediately after the header and (if necessary) color table.
  size_t img_data_offset_;

  // The BMP info header.
  BitmapInfoHeader info_header_;

  // True if this is an OS/2 1.x (aka Windows 2.x) BMP.  The struct
  // layouts for this type of BMP are slightly different from the later,
  // more common formats.
  bool is_os21x_;

  // True if this is an OS/2 2.x BMP.  The meanings of compression types 3
  // and 4 for this type of BMP differ from Windows V3+ BMPs.
  //
  // This will be falsely negative in some cases, but only ones where the
  // way we misinterpret the data is irrelevant.
  bool is_os22x_;

  // True if the BMP is not vertically flipped, that is, the first line of
  // raster data in the file is the top line of the image.
  bool is_top_down_;

  // These flags get set to false as we finish each processing stage.
  bool need_to_process_bitmasks_;
  bool need_to_process_color_table_;

  // Masks/offsets for the color values for non-palette formats. These are
  // bitwise, with array entries 0, 1, 2, 3 corresponding to R, G, B, A.
  // These are uninitialized (and ignored) for images with less than 16bpp.
  uint32_t bit_masks_[4];

  // Right shift values, meant to be applied after the masks. We need to shift
  // the bitfield values down from their offsets into the 32 bits of pixel
  // data, as well as truncate the least significant bits of > 8-bit fields.
  int bit_shifts_right_[4];

  // We use a lookup table to convert < 8-bit values into 8-bit values. The
  // values in the table are "round(val * 255.0 / ((1 << n) - 1))" for an
  // n-bit source value. These elements are set to 0 for 8-bit sources.
  const uint8_t* lookup_table_addresses_[4];

  // The color palette, for paletted formats.
  Vector<RGBTriple> color_table_;

  // The coordinate to which we've decoded the image.
  IntPoint coord_;

  // Variables that track whether we've seen pixels with alpha values != 0
  // and == 0, respectively.  See comments in ProcessNonRLEData() on how
  // these are used.
  bool seen_non_zero_alpha_pixel_;
  bool seen_zero_alpha_pixel_;

  // BMPs-in-ICOs have a few differences from standalone BMPs, so we need to
  // know if we're in an ICO container.
  bool is_in_ico_;

  // ICOs store a 1bpp "mask" immediately after the main bitmap image data
  // (and, confusingly, add its height to the biHeight value in the info
  // header, thus doubling it). If |is_in_ico_| is true, this variable tracks
  // whether we've begun decoding this mask yet.
  bool decoding_and_mask_;
};

}  // namespace blink

#endif
