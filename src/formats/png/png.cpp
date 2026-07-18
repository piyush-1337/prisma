#include <prisma/formats/png/png.hpp>

namespace prisma::format::png {

std::expected<PngImageHeader, std::string>
parse_header(std::span<const uint8_t> file_data) {
  size_t offset = 8;
  auto chunk = PngChunk::from_bytes(file_data, offset);
  if (!chunk)
    return std::unexpected(chunk.error());
  return PngImageHeader::from_chunk(*chunk);
}

}
