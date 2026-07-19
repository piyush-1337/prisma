#include <bit>
#include <format>
#include <prisma/codecs/image/png/png.hpp>
#include <zlib.h>

namespace prisma::codec::png {

std::expected<PngImageHeader, std::string>
parse_header(std::span<const uint8_t> file_data) {
  size_t offset = 8;
  auto chunk = PngChunk::from_bytes(file_data, offset);
  if (!chunk)
    return std::unexpected(chunk.error());
  return PngImageHeader::from_chunk(*chunk);
}

std::expected<core::Image, std::string>
decode(std::span<const uint8_t> file_data, core::Image &image) {
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

  image.width = header.width;
  image.height = header.height;
  image.channels = channels;
  image.pixels.resize(header.width * header.height * channels);

  auto paeth = [](int a, int b, int c) -> uint8_t {
    int p = a + b - c;
    int pa = std::abs(p - a);
    int pb = std::abs(p - b);
    int pc = std::abs(p - c);
    if (pa <= pb && pa <= pc)
      return static_cast<uint8_t>(a);
    if (pb <= pc)
      return static_cast<uint8_t>(b);
    return static_cast<uint8_t>(c);
  };

  for (uint32_t y{}; y < header.height; ++y) {
    size_t row_idx = y * (1 + row_bytes);
    uint8_t filter = decompressed_data[row_idx];
    size_t data_idx = row_idx + 1;
    size_t raw_row_idx = y * row_bytes;

    for (uint32_t x{}; x < row_bytes; ++x) {
      uint8_t byte = decompressed_data[data_idx + x];

      uint8_t left =
          (x >= channels) ? image.pixels[raw_row_idx + x - channels] : 0;
      uint8_t up = (y > 0) ? image.pixels[raw_row_idx - row_bytes + x] : 0;
      uint8_t up_left =
          (y > 0 && x >= channels)
              ? image.pixels[raw_row_idx - row_bytes + x - channels]
              : 0;

      switch (filter) {
      case 0:
        break;
      case 1: {
        byte += left;
        break;
      }
      case 2: {
        byte += up;
        break;
      }
      case 3: {
        byte += (left + up) >> 1;
        break;
      }
      case 4: {
        byte += paeth(left, up, up_left);
        break;
      }
      default:
        return std::unexpected(
            std::format("unknown png filter type: {}", filter));
      }

      image.pixels[raw_row_idx + x] = byte;
    }
  }

  return image;
}

void write_chunk(std::vector<uint8_t> &out, const uint8_t type[4],
                 std::span<const uint8_t> data) {

  uint32_t length = data.size();
  if constexpr (std::endian::native == std::endian::little) {
    length = std::byteswap(length);
  }
  auto *ptr = reinterpret_cast<const uint8_t *>(&length);
  out.insert(out.end(), ptr, ptr + 4);

  size_t crc_start = out.size();

  out.insert(out.end(), type, type + 4);
  out.insert(out.end(), data.begin(), data.end());

  uLong crc = crc32(0L, Z_NULL, 0);
  crc = crc32(crc, out.data() + crc_start, out.size() - crc_start);

  uint32_t crcu32 = static_cast<uint32_t>(crc);
  if constexpr (std::endian::native == std::endian::little) {
    crcu32 = std::byteswap(crcu32);
  }

  ptr = reinterpret_cast<const uint8_t *>(&crcu32);
  out.insert(out.end(), ptr, ptr + 4);
}

std::expected<PngImage, std::string> encode(const core::Image &image) {
  uint32_t raw_row_bytes = image.width * image.channels;
  size_t data_size = image.height * (1 + raw_row_bytes);
  std::vector<uint8_t> data(data_size);

  for (uint32_t y{}; y < image.height; ++y) {
    size_t raw_idx = y * raw_row_bytes;
    size_t data_idx = y * (1 + raw_row_bytes);

    data[data_idx] = 0;
    std::memcpy(&data[data_idx + 1], &image.pixels[raw_idx], raw_row_bytes);
  }

  uLongf dest_len = compressBound(static_cast<uLongf>(data_size));
  std::vector<uint8_t> compressed_data(dest_len);

  int zres =
      compress(compressed_data.data(), &dest_len, data.data(), data_size);
  if (zres != Z_OK)
    return std::unexpected(
        std::format("compression failed with zlib error: {}", zres));

  compressed_data.resize(dest_len);

  PngImage image_out;

  std::vector<uint8_t> out;
  //      magic + IHDR + IDAT + data + IEND
  out.reserve(8 + 25 + (12 + compressed_data.size()) + 12);

  // write magic
  out.insert(out.end(), PngImage::magic, PngImage::magic + 8);

  // write IHDR
  uint32_t width = image.width;
  uint32_t height = image.height;
  if constexpr (std::endian::native == std::endian::little) {
    width = std::byteswap(width);
    height = std::byteswap(height);
  }
  std::vector<uint8_t> ihdr_data;
  ihdr_data.reserve(13);
  auto ptr = reinterpret_cast<const uint8_t *>(&width);
  ihdr_data.insert(ihdr_data.end(), ptr, ptr + 4);
  ptr = reinterpret_cast<const uint8_t *>(&height);
  ihdr_data.insert(ihdr_data.end(), ptr, ptr + 4);
  ihdr_data.push_back(8);
  ihdr_data.push_back(image.channels == 3 ? 2 : 6);
  ihdr_data.push_back(0);
  ihdr_data.push_back(0);
  ihdr_data.push_back(0);

  write_chunk(out, ihdr_type, ihdr_data);
  write_chunk(out, idat_type, compressed_data);
  write_chunk(out, iend_type, {});

  image_out.compressed_data = std::move(out);
  return image_out;
}

} // namespace prisma::codec::png
