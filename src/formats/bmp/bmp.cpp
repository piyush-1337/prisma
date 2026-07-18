#include <cstring>
#include <prisma/formats/bmp/bmp.hpp>

namespace prisma::format::bmp {

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

std::expected<raw::RawImage, std::string>
encode(std::span<const uint8_t> file_data) {
  auto result = format::bmp::parse_header(file_data);
  if (!result)
    return std::unexpected(result.error());

  auto [file_header, info_header] = *result;

  uint32_t data_offset = file_header.data_offset;

  uint32_t width = info_header.width;
  int32_t height = info_header.height;

  uint16_t bpp = info_header.cdepth;
  if (bpp != 24 && bpp != 32) {
    return std::unexpected("only 24 and 32 bit bmps are currently supported");
  }

  uint16_t Bpp = bpp / 8;

  // should be multiple of 4
  uint32_t width_bytes = ((width * Bpp) + 3) & ~3;

  for (int32_t y = height - 1; y >= 0; --y) {
    for (int32_t x{}; x < width; ++x) {
      //                                        to the row       +   number of
      //                                        Bpp (usually BGR - 3)
      size_t index = file_header.data_offset + (y * width_bytes) + (x * Bpp);

      uint8_t b = file_data[index];
      uint8_t g = file_data[index + 1];
      uint8_t r = file_data[index + 2];

    }
  }

  return {};
}

} // namespace prisma::format::bmp
