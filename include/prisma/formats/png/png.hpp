#pragma once

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <expected>
#include <prisma/formats/raw/raw.hpp>
#include <span>
#include <string>
#include <vector>

namespace prisma::format::png {

struct PngChunkHeader {
  /*
   * A four-byte unsigned integer giving the number of bytes in the chunk's
   * data field. The length counts only the data field, not itself, the chunk
   * type, or the CRC. Zero is a valid length. Although encoders and decoders
   * should treat the length as unsigned, its value shall not exceed 231-1
   * bytes.
   */
  uint32_t length;

  /*
   * A sequence of four bytes defining the chunk type. Each byte of a chunk type
   * is restricted to the decimal values 65 to 90 and 97 to 122. These
   * correspond to the uppercase and lowercase ISO 646 letters (A-Z and a-z)
   * respectively for convenience in description and examination of PNG
   * datastreams. Encoders and decoders shall treat the chunk types as fixed
   * binary values, not character strings. For example, it would not be correct
   * to represent the chunk type IDAT by the equivalents of those letters in the
   * UCS 2 character set.
   */
  uint8_t chunk_type[4]; // not including the null-byte, use properly

  static std::expected<PngChunkHeader, std::string>
  from_bytes(std::span<const uint8_t> bytes, size_t &offset) {
    if (offset + 8 > bytes.size()) {
      return std::unexpected("not enough bytes to read png chunk header");
    }

    PngChunkHeader header;

    std::memcpy(&header.length, bytes.data() + offset, sizeof(header.length));
    if constexpr (std::endian::native == std::endian::little) {
      header.length = std::byteswap(header.length);
    }
    offset += 4;

    std::memcpy(header.chunk_type, bytes.data() + offset, 4);
    offset += 4;

    return header;
  }
};

struct PngChunk {
  PngChunkHeader header;

  /*
   * The data bytes appropriate to the chunk type, if any. This field can be of
   * zero length.
   */
  std::span<const uint8_t> data;

  /*
   * A four-byte CRC (Cyclic Redundancy Code) calculated on the preceding bytes
   * in the chunk, including the chunk type field and chunk data fields, but not
   * including the length field. The CRC can be used to check for corruption of
   * the data. The CRC is always present, even for chunks containing no data.
   */
  uint32_t crc;

  static std::expected<PngChunk, std::string>
  from_bytes(std::span<const uint8_t> bytes, size_t &offset) {
    auto header_res = PngChunkHeader::from_bytes(bytes, offset);
    if (!header_res)
      return std::unexpected(header_res.error());

    PngChunk chunk;
    chunk.header = *header_res;

    if (offset + chunk.header.length + 4 > bytes.size()) {
      return std::unexpected("chunk length exceeds buffer");
    }

    chunk.data = bytes.subspan(offset, chunk.header.length);
    offset += chunk.header.length;

    std::memcpy(&chunk.crc, bytes.data() + offset, 4);
    if constexpr (std::endian::native == std::endian::little) {
      chunk.crc = std::byteswap(chunk.crc);
    }
    offset += 4;

    return chunk;
  }
};

struct PngImageHeader {
  uint32_t width;
  uint32_t height;

  /*
   * Bit depth restrictions for each colour type are imposed to simplify
   * implementations and to prohibit combinations that do not compress well. The
   * allowed combinations are defined in
   * ![Table](https://www.w3.org/TR/2003/REC-PNG-20031110/#table111).
   */
  uint8_t bit_depth;

  /*
   * Colour type is a single-byte integer that defines the PNG image type. Valid
   * values are 0, 2, 3, 4, and 6.
   */
  uint8_t color_type;

  /*
   * Compression method is a single-byte integer that indicates the method used
   * to compress the image data. Only compression method 0 (deflate/inflate
   * compression with a sliding window of at most 32768 bytes) is defined in
   * this International Standard. All conforming PNG images shall be compressed
   * with this scheme.
   */
  uint8_t compression_method;

  /*
   * Filter method is a single-byte integer that indicates the preprocessing
   * method applied to the image data before compression. Only filter method 0
   * (adaptive filtering with five basic filter types) is defined in this
   * International Standard.
   */
  uint8_t filter_method;

  /*
   * Interlace method is a single-byte integer that indicates the transmission
   * order of the image data. Two values are defined in this International
   * Standard: 0 (no interlace) or 1 (Adam7 interlace).
   */
  uint8_t interlace_method;

  static std::expected<PngImageHeader, std::string> from_chunk(PngChunk chunk) {
    uint8_t type[4] = {73, 72, 68, 82};
    if (!std::ranges::equal(chunk.header.chunk_type, type)) {
      return std::unexpected("chunk is not of type IHDR");
    }

    auto data = chunk.data;
    if (data.size() < 13) {
      return std::unexpected("ihdr payload too small");
    }
    size_t offset = 0;

    PngImageHeader header;
    std::memcpy(&header.width, data.data() + offset, 4);
    if constexpr (std::endian::native == std::endian::little) {
      header.width = std::byteswap(header.width);
    }
    offset += 4;

    std::memcpy(&header.height, data.data() + offset, 4);
    if constexpr (std::endian::native == std::endian::little) {
      header.height = std::byteswap(header.height);
    }
    offset += 4;

    header.bit_depth = data[offset++];
    header.color_type = data[offset++];
    header.compression_method = data[offset++];
    header.filter_method = data[offset++];
    header.interlace_method = data[offset++];

    return header;
  }
};

// optional fields are ommited
struct PngImage {
  static constexpr uint8_t magic[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
  PngImageHeader ihdr;
  std::vector<uint8_t> compressed_data;
};

std::expected<PngImageHeader, std::string>
parse_header(std::span<const uint8_t> file_data);

std::expected<raw::RawImage, std::string>
decode(std::span<const uint8_t> file_data);

std::expected<PngImage, std::string> encode(const raw::RawImage &raw_image);

} // namespace prisma::format::png
