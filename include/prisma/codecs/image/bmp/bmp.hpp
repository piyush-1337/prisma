#pragma once

#include <cstdint>
#include <expected>
#include <prisma/core/image.hpp>
#include <span>
#include <string>

namespace prisma::codec::bmp {

#pragma pack(push, 1)

struct BmpFileHeader {
  /// The header field used to identify the BMP and DIB file is 0x42 0x4D in
  /// hexadecimal, same as BM in ASCII.
  uint8_t magic[2];
  /// The size of the BMP file in bytes
  uint32_t file_size;
  /// Reserved; actual value depends on the application that creates the image,
  /// if created manually can be 0
  uint16_t reserved1;
  /// Reserved; actual value depends on the application that creates the image,
  /// if created manually can be 0
  uint16_t reserved2;
  /// The offset, i.e. starting address, of the byte where the bitmap image data
  /// (pixel array) can be found.
  uint32_t data_offset;
};

struct BmpInfoHeader {
  /// the size of this header, in bytes (40)
  uint32_t header_size;
  /// the bitmap width in pixels (signed integer)
  int32_t width;
  /// the bitmap height in pixels (signed integer)
  int32_t height;
  /// the number of color planes (must be 1)
  uint16_t npanes;
  /// the number of bits per pixel, which is the color depth of the image.
  /// Typical values are 1, 4, 8, 16, 24 and 32.
  uint16_t cdepth;
  /// the compression method being used.
  uint32_t compression_method;
  /// the image size. This is the size of the raw bitmap data; a dummy 0 can be
  /// given for BI_RGB bitmaps.
  uint32_t image_size;
  /// the horizontal resolution of the image. (pixel per metre, signed integer)
  int32_t h_res;
  /// the vertical resolution of the image. (pixel per metre, signed integer)
  int32_t v_res;
  /// the number of colors in the color palette, or 0 to default to 2n
  uint32_t ncolors;
  /// the number of important colors used, or 0 when every color is important;
  /// generally ignored
  uint32_t nimpcolors;
};

#pragma pack(pop)

struct BmpImage {
  BmpFileHeader file_header;
  BmpInfoHeader info_header;
  std::vector<uint8_t> pixels;
};

std::expected<std::pair<BmpFileHeader, BmpInfoHeader>, std::string>
parse_header(std::span<const uint8_t> file_data);

std::expected<core::Image, std::string>
decode(std::span<const uint8_t> file_data);

std::expected<BmpImage, std::string> encode(const core::Image &image);
                                        

} // namespace prisma::codec::bmp
