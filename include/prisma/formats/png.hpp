#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <expected>
#include <span>
#include <string>

namespace prisma {

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
      return std::unexpected(
          "chunk length exceeds buffer");
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

} // namespace prisma
