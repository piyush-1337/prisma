#include <cstring>
#include <prisma/formats/bmp/bmp.hpp>
#include <prisma/formats/png/png.hpp>
#include <prisma/parser.hpp>

namespace prisma {

namespace format::bmp {

std::expected<std::pair<BmpFileHeader, BmpInfoHeader>, std::string>
parse_header(std::span<const uint8_t> file_data) {

  if (file_data.size() < (sizeof(BmpFileHeader) + sizeof(BmpInfoHeader))) {
    return std::unexpected("File too small to contain BMP headers");
  }

  BmpFileHeader file_header;
  BmpInfoHeader info_header;

  std::memcpy(&file_header, file_data.data(), sizeof(BmpFileHeader));
  std::memcpy(&info_header, file_data.data() + sizeof(BmpFileHeader),
              sizeof(BmpInfoHeader));

  return std::make_pair(file_header, info_header);
}

} // namespace format::bmp

namespace format::png {

std::expected<PngImageHeader, std::string>
parse_header(std::span<const uint8_t> file_data) {
  size_t offset = 8;
  auto chunk = PngChunk::from_bytes(file_data, offset);
  if (!chunk)
    return std::unexpected(chunk.error());
  return PngImageHeader::from_chunk(*chunk);
}

} // namespace format::png

} // namespace prisma
