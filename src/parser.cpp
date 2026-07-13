#include <cstring>
#include <print>
#include <prisma/formats/bmp.hpp>
#include <prisma/parser.hpp>

namespace prisma {

std::expected<void, std::string>
print_bmp_info(std::span<const uint8_t> file_data) {

  if (file_data.size() < (sizeof(BmpFileHeader) + sizeof(BmpInfoHeader))) {
    return std::unexpected("File too small to contain BMP headers");
  }

  BmpFileHeader file_header;
  BmpInfoHeader info_header;

  std::memcpy(&file_header, file_data.data(), sizeof(BmpFileHeader));
  std::memcpy(&info_header, file_data.data() + sizeof(BmpFileHeader),
              sizeof(BmpInfoHeader));

  std::println("Found format: BMP (bitmap)");
  std::println("File Size:    {} bytes", file_header.file_size);
  std::println("Resolution:   {}x{}", info_header.width, info_header.height);
  std::println("Color Depth:  {}-bit", info_header.cdepth);
  std::println("Data Offset:  Byte {}", file_header.data_offset);

  return {};
}

} // namespace prisma
