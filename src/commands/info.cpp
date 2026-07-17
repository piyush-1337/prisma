#include <cstdint>
#include <print>
#include <prisma/commands/info.hpp>
#include <prisma/parser.hpp>
#include <span>

namespace prisma {

std::expected<void, std::string>
print_bmp_info(std::span<const uint8_t> file_data) {
  auto result = parse_bmp_header(file_data);
  if (!result) {
    return std::unexpected(result.error());
  }

  auto [file_header, info_header] = *result;

  std::println("Found format: BMP (bitmap)");
  std::println("File Size:    {} bytes", file_header.file_size);
  std::println("Resolution:   {}x{}", info_header.width, info_header.height);
  std::println("Color Depth:  {}-bit", info_header.cdepth);
  std::println("Data Offset:  Byte {}", file_header.data_offset);

  return {};
}

std::expected<void, std::string>
print_png_info(std::span<const uint8_t> file_data) {
  auto result = parse_png_header(file_data);
  if (!result) {
    return std::unexpected(result.error());
  }

  auto header = *result;

  std::println("Found format: PNG");
  std::println("Resolution:   {}x{}", header.width, header.height);
  std::println("Color Depth:  {}-bit", header.bit_depth);

  return {};
}

} // namespace prisma
