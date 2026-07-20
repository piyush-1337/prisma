#include <cstdint>
#include <print>
#include <prisma/codecs/audio/wav.hpp>
#include <prisma/codecs/image/bmp/bmp.hpp>
#include <prisma/codecs/image/png/png.hpp>
#include <prisma/commands/info.hpp>
#include <span>

namespace prisma {

namespace codec::bmp {

std::expected<void, std::string>
print_info(std::span<const uint8_t> file_data) {
  auto result = parse_header(file_data);
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

} // namespace codec::bmp

namespace codec::png {

std::expected<void, std::string>
print_info(std::span<const uint8_t> file_data) {
  auto result = parse_header(file_data);
  if (!result) {
    return std::unexpected(result.error());
  }

  auto header = *result;

  auto color_layout = [](uint8_t color_type) {
    switch (color_type) {
    case 0:
      return "Grayscale";
    case 2:
      return "RGB";
    case 3:
      return "Indexed";
    case 4:
      return "Grayscale + Alpha";
    case 6:
      return "RGBA";
    default:
      return "Unknown";
    }
  };

  std::println("Found format: PNG");
  std::println("Resolution:   {}x{}", header.width, header.height);
  std::println("Color Depth:  {}-bit", header.bit_depth);
  std::println("Color Layout: {}", color_layout(header.color_type));

  return {};
}

} // namespace codec::png

namespace codec::wav {

std::expected<void, std::string>
print_info(std::span<const uint8_t> file_data) {
  size_t offset = 0;

  MasterRiffHeader master_header =
      MasterRiffHeader::from_bytes(file_data, offset);

  while (offset < file_data.size()) {
    ChunkHeader chunk_header = ChunkHeader::from_bytes(file_data, offset);

    if (std::memcmp(chunk_header.id, "fmt ", 4) == 0) {
      FmtChunk fmt_chunk = FmtChunk::from_bytes(file_data, offset);

      std::println("Format:       {}", fmt_chunk.audio_format == 1
                                           ? "PCM (Uncompressed)"
                                           : "Compressed/Unknown");
      std::println("Channels:     {} ({})", fmt_chunk.num_channels,
                   fmt_chunk.num_channels == 1 ? "Mono" : "Stereo");
      std::println("Sample Rate:  {} Hz", fmt_chunk.sample_rate);
      std::println("Bit Depth:    {}-bit", fmt_chunk.bits_per_sample);

      return {};
    } else {
      offset += chunk_header.size;
    }
  }

  return std::unexpected("unable to parse wav file");
}

} // namespace codec::wav

} // namespace prisma
