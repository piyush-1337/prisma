#include <bit>
#include <cstdint>
#include <cstring>
#include <prisma/format.hpp>
#include <prisma/codecs/image/bmp/bmp.hpp>
#include <prisma/core/image.hpp>

namespace prisma::codec::bmp {

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

  if constexpr (std::endian::native == std::endian::big) {
    file_header.file_size = std::byteswap(file_header.file_size);
    file_header.reserved1 = std::byteswap(file_header.reserved1);
    file_header.reserved2 = std::byteswap(file_header.reserved2);
    file_header.data_offset = std::byteswap(file_header.data_offset);

    info_header.header_size = std::byteswap(info_header.header_size);
    info_header.width = std::byteswap(info_header.width);
    info_header.height = std::byteswap(info_header.height);
    info_header.npanes = std::byteswap(info_header.npanes);
    info_header.cdepth = std::byteswap(info_header.cdepth);
    info_header.compression_method =
        std::byteswap(info_header.compression_method);
    info_header.image_size = std::byteswap(info_header.image_size);
    info_header.h_res = std::byteswap(info_header.h_res);
    info_header.v_res = std::byteswap(info_header.v_res);
    info_header.ncolors = std::byteswap(info_header.ncolors);
    info_header.nimpcolors = std::byteswap(info_header.nimpcolors);
  }

  return std::make_pair(file_header, info_header);
}

std::expected<core::Image, std::string>
decode(std::span<const uint8_t> file_data) {
  auto result = codec::bmp::parse_header(file_data);
  if (!result)
    return std::unexpected(result.error());

  auto [file_header, info_header] = *result;

  uint32_t data_offset = file_header.data_offset;

  uint32_t width = info_header.width;
  uint32_t height = std::abs(info_header.height);
  bool is_reverse = info_header.height < 0;

  uint16_t bpp = info_header.cdepth;
  if (bpp != 24 && bpp != 32) {
    return std::unexpected("only 24 and 32 bit bmps are currently supported");
  }

  uint16_t bytes_per_pixel = bpp / 8;

  core::Image image;
  image.width = width;
  image.height = height;
  image.channels = bytes_per_pixel;
  image.pixels.resize(width * height * bytes_per_pixel);

  // should be multiple of 4
  uint32_t bmp_row_bytes = ((width * bytes_per_pixel) + 3) & ~3;
  uint32_t raw_row_bytes = width * bytes_per_pixel;

  if (file_header.data_offset + (height * bmp_row_bytes) > file_data.size()) {
    return std::unexpected("BMP file too small or corrupted");
  }

  for (uint32_t y{}; y < height; ++y) {
    uint32_t bmp_y = is_reverse ? y : (height - 1) - y;

    for (uint32_t x{}; x < width; ++x) {
      size_t bmp_index = file_header.data_offset + (bmp_y * bmp_row_bytes) +
                         (x * bytes_per_pixel);
      size_t raw_index = (y * raw_row_bytes) + (x * bytes_per_pixel);

      uint8_t b = file_data[bmp_index];
      uint8_t g = file_data[bmp_index + 1];
      uint8_t r = file_data[bmp_index + 2];

      image.pixels[raw_index] = r;
      image.pixels[raw_index + 1] = g;
      image.pixels[raw_index + 2] = b;

      if (bytes_per_pixel == 4) {
        image.pixels[raw_index + 3] = file_data[bmp_index + 3];
      }
    }
  }

  return image;
}

std::expected<BmpImage, std::string> encode(const core::Image &image) {
  uint32_t width = image.width;
  uint32_t height = image.height;
  uint16_t channels = image.channels;

  uint32_t raw_row_bytes = image.width * image.channels;
  uint32_t bmp_row_bytes = (raw_row_bytes + 3) & ~3;
  uint32_t pixels_size = bmp_row_bytes * image.height;
  uint32_t header_size = sizeof(BmpFileHeader) + sizeof(BmpInfoHeader);

  std::vector<uint8_t> bmp_pixels(bmp_row_bytes * height, 0);

  for (uint32_t y{}; y < height; ++y) {
    for (uint32_t x{}; x < width; ++x) {
      size_t raw_idx = (y * raw_row_bytes) + (x * channels);
      size_t bmp_idx = (y * bmp_row_bytes) + (x * channels);

      uint8_t r = image.pixels[raw_idx];
      uint8_t g = image.pixels[raw_idx + 1];
      uint8_t b = image.pixels[raw_idx + 2];

      bmp_pixels[bmp_idx] = b;
      bmp_pixels[bmp_idx + 1] = g;
      bmp_pixels[bmp_idx + 2] = r;

      if (channels == 4) {
        bmp_pixels[bmp_idx + 3] = image.pixels[raw_idx + 3];
      }
    }
  }

  BmpFileHeader file_header{
      .magic = {'B', 'M'},
      .file_size = header_size + pixels_size,
      .reserved1 = 0,
      .reserved2 = 0,
      .data_offset = header_size,
  };

  BmpInfoHeader info_header = {
      .header_size = sizeof(BmpInfoHeader),
      .width = static_cast<int32_t>(image.width),
      .height = -static_cast<int32_t>(image.height), // easy
      .npanes = 1,
      .cdepth = static_cast<uint16_t>(image.channels * 8),
      .compression_method = 0,
      .image_size = pixels_size,
      .h_res = 2835, // 72 dpi (standard)
      .v_res = 2835,
      .ncolors = 0,
      .nimpcolors = 0};

  return BmpImage{.file_header = file_header,
                  .info_header = info_header,
                  .pixels = bmp_pixels};
}

} // namespace prisma::codec::bmp
