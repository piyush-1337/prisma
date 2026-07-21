#include <cstddef>
#include <prisma/codecs/audio/wav.hpp>

namespace prisma::codec::wav {

std::expected<FmtChunk, std::string>
parse_fmt_chunk(std::span<const uint8_t> file_data) {
  size_t offset = 0;

  MasterRiffHeader master_header =
      MasterRiffHeader::from_bytes(file_data, offset);

  while (offset < file_data.size()) {
    ChunkHeader chunk_header = ChunkHeader::from_bytes(file_data, offset);

    if (std::memcmp(chunk_header.id, "fmt ", 4) == 0) {
      FmtChunk fmt_chunk = FmtChunk::from_bytes(file_data, offset);

      return fmt_chunk;
    } else {
      offset += (chunk_header.size + 1) & ~1;
    }
  }

  return std::unexpected("fmt chunk not found");
}

std::expected<core::Audio, std::string>
decode(std::span<const uint8_t> file_data) {

  size_t offset = 0;
  MasterRiffHeader master_header =
      MasterRiffHeader::from_bytes(file_data, offset);

  auto fmt_chunk_res = parse_fmt_chunk(file_data);
  if (!fmt_chunk_res)
    return std::unexpected(fmt_chunk_res.error());

  FmtChunk fmt_chunk = *fmt_chunk_res;

  core::Audio audio;
  audio.channels = fmt_chunk.num_channels;
  audio.sample_rate = fmt_chunk.sample_rate;
  audio.bit_depth = fmt_chunk.bits_per_sample;

  while (offset < file_data.size()) {
    ChunkHeader chunk_header = ChunkHeader::from_bytes(file_data, offset);

    if (std::memcmp(chunk_header.id, "data", 4) == 0) {
      audio.pcm.resize(chunk_header.size);
      std::memcpy(audio.pcm.data(), file_data.data() + offset,
                  chunk_header.size);
      return audio;
    } else {
      offset += (chunk_header.size + 1) & ~1;
    }
  }

  return std::unexpected("coudn't find data or fmt chunk");
}

} // namespace prisma::codec::wav
