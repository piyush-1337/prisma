#include <bit>
#include <cstdint>
#include <fstream>
#include <prisma/commands/convert.hpp>
#include <prisma/format.hpp>
#include <prisma/formats/bmp/bmp.hpp>
#include <prisma/formats/raw/raw.hpp>

namespace prisma {

std::expected<void, std::string>
convert(std::span<const uint8_t> file_data, Filters filters,
        const std::filesystem::path &out_path) {

  auto src = identify_format(file_data);
  auto dest = identify_format_from_ext(out_path);

  format::raw::RawImage raw_image;

  switch (src) {
  case Format::BMP: {
    auto res = format::bmp::decode(file_data);
    if (!res)
      return std::unexpected(res.error());
    raw_image = std::move(*res);
      break;
  }
  case Format::WAV:
  case Format::FLAC:
  case Format::PNG:
  case Format::UNKNOWN:
    return std::unexpected("source format unknown/unimplemented");
  }

  if (filters.grayscale) {
    format::raw::apply_grayscale(raw_image);
  }
  if (filters.invert) {
    format::raw::apply_invert(raw_image);
  }

  switch (dest) {
  case Format::BMP: {
    auto res = format::bmp::encode(raw_image);
    if (!res)
      return std::unexpected(res.error());
    format::bmp::BmpImage image = std::move(*res);

    std::ofstream out(out_path, std::ios::binary);

    if constexpr (std::endian::native == std::endian::little) {

      out.write(reinterpret_cast<const char *>(&image.file_header),
                sizeof(format::bmp::BmpFileHeader));
      out.write(reinterpret_cast<const char *>(&image.info_header),
                sizeof(format::bmp::BmpInfoHeader));
    } else {
      format::bmp::BmpFileHeader le_file_header = image.file_header;
      le_file_header.file_size = std::byteswap(le_file_header.file_size);
      le_file_header.reserved1 = std::byteswap(le_file_header.reserved1);
      le_file_header.reserved2 = std::byteswap(le_file_header.reserved2);
      le_file_header.data_offset = std::byteswap(le_file_header.data_offset);

      format::bmp::BmpInfoHeader le_info_header = image.info_header;
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
                sizeof(format::bmp::BmpFileHeader));
      out.write(reinterpret_cast<const char *>(&le_info_header),
                sizeof(format::bmp::BmpInfoHeader));
    }

    out.write(reinterpret_cast<const char *>(image.pixels.data()),
              image.pixels.size());

    return {};
  }
  case Format::WAV:
  case Format::FLAC:
  case Format::PNG:
  case Format::UNKNOWN:
    return std::unexpected("destination format unknown/unimplemented");
  }
}

} // namespace prisma
