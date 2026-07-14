#pragma once

#include <cstdint>
#include <expected>
#include <prisma/cli.hpp>
#include <prisma/parser.hpp>
#include <span>
#include <string>
namespace prisma {

template <typename PixelAction>
std::expected<void, std::string> process_bmp(std::span<const uint8_t> file_data,
                                             Filters filters,
                                             PixelAction &&action) {
  auto result = parse_bmp(file_data);
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
      //                    to the row       +   number of Bpp (usually BGR - 3)
      size_t index = file_header.data_offset + (y * width_bytes) + (x * Bpp);

      uint8_t b = file_data[index];
      uint8_t g = file_data[index + 1];
      uint8_t r = file_data[index + 2];

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

      /// returns r, g, b, end of row?, index into the file
      action(r, g, b, x == width - 1, index);
    }
  }

  return {};
}

} // namespace prisma
