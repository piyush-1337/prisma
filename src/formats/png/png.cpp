#include <format>
#include <prisma/formats/png/png.hpp>
#include <zlib.h>

namespace prisma::format::png {

std::expected<PngImageHeader, std::string>
parse_header(std::span<const uint8_t> file_data) {
  size_t offset = 8;
  auto chunk = PngChunk::from_bytes(file_data, offset);
  if (!chunk)
    return std::unexpected(chunk.error());
  return PngImageHeader::from_chunk(*chunk);
}

std::expected<raw::RawImage, std::string>
decode(std::span<const uint8_t> file_data) {
  size_t offset = 8;

  auto header_chunk = PngChunk::from_bytes(file_data, offset);
  if (!header_chunk)
    return std::unexpected(header_chunk.error());

  auto res = PngImageHeader::from_chunk(*header_chunk);
  if (!res)
    return std::unexpected(res.error());

  auto header = *res;
  if (header.interlace_method != 0)
    return std::unexpected("Adam7 interlace method not supported");

  std::vector<uint8_t> compressed_data;
  constexpr uint8_t idat_type[4] = {73, 68, 65, 84};
  constexpr uint8_t iend_type[4] = {73, 69, 78, 68};

  while (offset < file_data.size()) {
    auto chunk_res = PngChunk::from_bytes(file_data, offset);
    if (!chunk_res)
      return std::unexpected(chunk_res.error());

    auto chunk = *chunk_res;
    if (std::ranges::equal(chunk.header.chunk_type, idat_type)) {
      compressed_data.append_range(chunk.data);
    } else if (std::ranges::equal(chunk.header.chunk_type, iend_type)) {
      break;
    }
  }

  if (header.bit_depth != 8)
    return std::unexpected("only 8-bit depth is supported");

  uint32_t channels;
  if (header.color_type == 2)
    channels = 3;
  else if (header.color_type == 6)
    channels = 4;
  else
    return std::unexpected("only rgb and rgba are supported for png");

  uint32_t row_bytes = header.width * channels;
  size_t decompressed_size = header.height * (1 + row_bytes);

  std::vector<uint8_t> decompressed_data(decompressed_size);

  uLongf dest_len = static_cast<uLongf>(decompressed_data.size());
  int zres = uncompress(decompressed_data.data(), &dest_len,
                        compressed_data.data(), compressed_data.size());
  if (zres != Z_OK)
    return std::unexpected(
        std::format("decompression failed with zlib error code: {}", zres));

  if (dest_len != decompressed_size)
    return std::unexpected("decompressed data size mismatch");

  return std::unexpected("zlib worked");
}

std::expected<PngImage, std::string> encode(const raw::RawImage &raw_image);

} // namespace prisma::format::png
