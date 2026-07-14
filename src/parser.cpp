#include <cstring>
#include <prisma/formats/bmp.hpp>
#include <prisma/parser.hpp>

namespace prisma {

std::expected<std::pair<BmpFileHeader, BmpInfoHeader>, std::string>
parse_bmp(std::span<const uint8_t> file_data) {

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

} // namespace prisma
