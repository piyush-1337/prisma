#include <bit>
#include <cstdint>
#include <fstream>
#include <prisma/commands/convert.hpp>
#include <prisma/format.hpp>
#include <prisma/codecs/image/bmp/bmp.hpp>
#include <prisma/codecs/image/png/png.hpp>
#include <prisma/core/image.hpp>

namespace prisma {

std::expected<void, std::string>
convert(std::span<const uint8_t> file_data, Filters filters,
        const std::filesystem::path &out_path) {

  auto src = identify_format(file_data);
  auto dest = identify_format_from_ext(out_path);

  core::Image image;

  switch (src) {
  case Format::BMP: {
    auto res = codec::bmp::decode(file_data);
    if (!res)
      return std::unexpected(res.error());
    image = std::move(*res);
    break;
  }
  case Format::WAV:
  case Format::FLAC:
  case Format::PNG: {
    core::Image image_temp;
    auto res = codec::png::decode(file_data, image_temp);
    if (!res)
      return std::unexpected(res.error());
    image = std::move(*res);
    break;
  }
  case Format::UNKNOWN:
    return std::unexpected("source format unknown/unimplemented");
  }

  if (filters.grayscale) {
    core::apply_grayscale(image);
  }
  if (filters.invert) {
    core::apply_invert(image);
  }

  switch (dest) {
  case Format::BMP: {
    auto res = codec::bmp::encode(image);
    if (!res)
      return std::unexpected(res.error());
    codec::bmp::BmpImage image_out = std::move(*res);

    std::ofstream out(out_path, std::ios::binary);
    if (!out.is_open()) {
      std::unexpected(std::format("failed to open file for wrinting: {}",
                                  out_path.string()));
    }

    if constexpr (std::endian::native == std::endian::little) {

      out.write(reinterpret_cast<const char *>(&image_out.file_header),
                sizeof(codec::bmp::BmpFileHeader));
      out.write(reinterpret_cast<const char *>(&image_out.info_header),
                sizeof(codec::bmp::BmpInfoHeader));
    } else {
      codec::bmp::BmpFileHeader le_file_header = image_out.file_header;
      le_file_header.file_size = std::byteswap(le_file_header.file_size);
      le_file_header.reserved1 = std::byteswap(le_file_header.reserved1);
      le_file_header.reserved2 = std::byteswap(le_file_header.reserved2);
      le_file_header.data_offset = std::byteswap(le_file_header.data_offset);

      codec::bmp::BmpInfoHeader le_info_header = image_out.info_header;
      le_info_header.header_size = std::byteswap(le_info_header.header_size);
      le_info_header.width = std::byteswap(le_info_header.width);
      le_info_header.height = std::byteswap(le_info_header.height);
      le_info_header.npanes = std::byteswap(le_info_header.npanes);
      le_info_header.cdepth = std::byteswap(le_info_header.cdepth);
      le_info_header.compression_method =
          std::byteswap(le_info_header.compression_method);
      le_info_header.image_size = std::byteswap(le_info_header.image_size);
      le_info_header.h_res = std::byteswap(le_info_header.h_res);
      le_info_header.v_res = std::byteswap(le_info_header.v_res);
      le_info_header.ncolors = std::byteswap(le_info_header.ncolors);
      le_info_header.nimpcolors = std::byteswap(le_info_header.nimpcolors);

      out.write(reinterpret_cast<const char *>(&le_file_header),
                sizeof(codec::bmp::BmpFileHeader));
      out.write(reinterpret_cast<const char *>(&le_info_header),
                sizeof(codec::bmp::BmpInfoHeader));
    }

    out.write(reinterpret_cast<const char *>(image_out.pixels.data()),
              image_out.pixels.size());

    return {};
  }
  case Format::WAV:
  case Format::FLAC:
  case Format::PNG: {
    auto res = codec::png::encode(image);
    if (!res)
      return std::unexpected(res.error());

    codec::png::PngImage image_out = std::move(*res);
    std::ofstream out(out_path, std::ios::binary);
    if (!out.is_open()) {
      std::unexpected(std::format("failed to open file for wrinting: {}",
                                  out_path.string()));
    }

    const char *buf =
        reinterpret_cast<const char *>(image_out.compressed_data.data());
    size_t size = image_out.compressed_data.size();

    out.write(buf, size);

    return {};
  }
  case Format::UNKNOWN:
    return std::unexpected("destination format unknown/unimplemented");
  }
}

} // namespace prisma
