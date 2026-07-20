#pragma once

#include <bit>
#include <cstdint>
#include <cstring>
#include <span>

namespace prisma::codec::wav {

#pragma pack(push, 1)

struct MasterRiffHeader {
  uint8_t magic[4] = {0x52, 0x49, 0x46, 0x46};
  // overall file size minus 8 bytes
  uint32_t file_size;
  uint8_t format[4] = {0x57, 0x41, 0x56, 0x45};

  static MasterRiffHeader from_bytes(std::span<const uint8_t> data,
                                     size_t &offset) {
    MasterRiffHeader header;
    std::memcpy(&header, data.data() + offset, sizeof(MasterRiffHeader));
    offset += sizeof(MasterRiffHeader);

    if (std::endian::native == std::endian::big) {
      header.swap_endianess();
    }
    return header;
  };

  void swap_endianess() { file_size = std::byteswap(file_size); }
};

struct ChunkHeader {
  uint8_t id[4];
  /// the size of payload of this chunk
  uint32_t size;

  static ChunkHeader from_bytes(std::span<const uint8_t> data, size_t &offset) {
    ChunkHeader header;
    std::memcpy(&header, data.data() + offset, sizeof(ChunkHeader));
    offset += sizeof(ChunkHeader);
    if (std::endian::native == std::endian::big) {
      header.swap_endianess();
    }
    return header;
  }

  void swap_endianess() { size = std::byteswap(size); }
};

struct FmtChunk {
  // 1 = PCM
  uint16_t audio_format;
  // 1 = Mono, 2 = Stereo
  uint16_t num_channels;
  // frequency
  uint32_t sample_rate;
  // sample_rate * num_channels * (bits_per_sample / 8)
  uint32_t byte_rate;
  // num_channels * (bits_per_sample / 8)
  uint16_t block_align;
  // e.g., 16
  uint16_t bits_per_sample;

  static FmtChunk from_bytes(std::span<const uint8_t> data, size_t &offset) {
    FmtChunk chunk;
    std::memcpy(&chunk, data.data() + offset, sizeof(FmtChunk));
    offset += sizeof(FmtChunk);
    if (std::endian::native == std::endian::big) {
      chunk.swap_endianess();
    }
    return chunk;
  }

  void swap_endianess() {
    audio_format = std::byteswap(audio_format);
    num_channels = std::byteswap(num_channels);
    sample_rate = std::byteswap(sample_rate);
    byte_rate = std::byteswap(byte_rate);
    block_align = std::byteswap(block_align);
    bits_per_sample = std::byteswap(bits_per_sample);
  }
};

#pragma pack(pop)

} // namespace prisma::codec::wav
