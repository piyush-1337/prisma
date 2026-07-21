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
      return {};
    } else {
      offset += chunk_header.size;
    }
  }

  return std::unexpected("fmt chunk not found");
}

} // namespace prisma::codec::wav
