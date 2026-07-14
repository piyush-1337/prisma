#include <cstddef>
#include <cstdint>
#include <print>
#include <prisma/commands/render.hpp>
#include <prisma/parser.hpp>

namespace prisma {

std::expected<void, std::string> render_bmp(std::span<const uint8_t> file_data,
                                            Filters filters) {
  auto result = parse_bmp(file_data);
  if (!result)
    return std::unexpected(result.error());

  auto [file_header, info_header] = *result;

  uint32_t data_offset = file_header.data_offset;
  auto raw_pixels = file_data.subspan(data_offset);

  uint32_t width = info_header.width;
  uint32_t height = info_header.height;

  // should be multiple of 4
  uint32_t width_bytes = ((width * 3) + 3) & ~3;

  for (int32_t y = height - 1; y >= 0; --y) {
    for (int32_t x{}; x < width; ++x) {
      //                    to the row       +   BRG
      size_t pixel_index = (y * width_bytes) + (x * 3);

      uint8_t b = raw_pixels[pixel_index];
      uint8_t g = raw_pixels[pixel_index + 1];
      uint8_t r = raw_pixels[pixel_index + 2];

      if (filters.invert) {
        b = 255 - b;
        g = 255 - g;
        r = 255 - r;
      }

      if (filters.grayscale) {
        uint8_t luminance = (r * 54 + g * 183 + b * 19) >> 8;

        b = luminance;
        g = luminance;
        r = luminance;
      }

      // ANSI Truecolor escape sequence
      std::print("\x1b[48;2;{};{};{}m  \x1b[0m", +r, +g, +b);
    }
    std::println();
  }

  return {};
}

} // namespace prisma
