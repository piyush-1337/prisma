#include <bit>
#include <cstdint>
#include <fstream>
#include <prisma/codecs/audio/wav.hpp>
#include <prisma/codecs/image/bmp.hpp>
#include <prisma/codecs/image/png.hpp>
#include <prisma/commands/convert.hpp>
#include <prisma/core/audio.hpp>
#include <prisma/core/image.hpp>
#include <prisma/format.hpp>

namespace prisma {

std::expected<void, std::string>
convert(std::span<const uint8_t> file_data, Filters filters,
        const std::filesystem::path &out_path) {

  auto src = identify_format(file_data);
  auto dest = identify_format_from_ext(out_path);

  MediaType src_type = get_media_type(src);
  MediaType dest_type = get_media_type(dest);

  if (src_type != dest_type)
    return std::unexpected("input and output are not of same media type");

  if (src_type == MediaType::IMAGE) {
    return convert_image(src, dest, file_data, filters.image_filters, out_path);
  } else if (src_type == MediaType::AUDIO) {
    return convert_audio(src, dest, file_data, filters.audio_filters, out_path);
  }

  return std::unexpected("unknown media type");
}

std::expected<void, std::string>
convert_image(Format src, Format dest, std::span<const uint8_t> file_data,
              ImageFilters image_filters,
              const std::filesystem::path &out_path) {
  core::Image image;

  switch (src) {
  case Format::BMP: {
    auto res = codec::bmp::decode(file_data);
    if (!res)
      return std::unexpected(res.error());
    image = std::move(*res);
    break;
  }
  case Format::PNG: {
    auto res = codec::png::decode(file_data);
    if (!res)
      return std::unexpected(res.error());
    image = std::move(*res);
    break;
  }
  default:
  case Format::UNKNOWN:
    return std::unexpected("source format unknown/unimplemented");
  }

  if (image_filters.grayscale) {
    core::apply_grayscale(image);
  }
  if (image_filters.invert) {
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

    if constexpr (std::endian::native == std::endian::big) {
      image_out.swap_endianess();
    }

    out.write(reinterpret_cast<const char *>(&image_out.file_header),
              sizeof(codec::bmp::BmpFileHeader));
    out.write(reinterpret_cast<const char *>(&image_out.info_header),
              sizeof(codec::bmp::BmpInfoHeader));
    out.write(reinterpret_cast<const char *>(image_out.pixels.data()),
              image_out.pixels.size());

    return {};
  }
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
  default:
  case Format::UNKNOWN:
    return std::unexpected("destination format unknown/unimplemented");
  }

  return std::unexpected("shouldn't reach here");
}

std::expected<void, std::string>
convert_audio(Format src, Format dest, std::span<const uint8_t> file_data,
              AudioFilers audio_filters,
              const std::filesystem::path &out_path) {

  core::Audio audio;
  switch (src) {
  case Format::WAV: {
    auto res = codec::wav::decode(file_data);
    if (!res)
      return std::unexpected(res.error());
    audio = std::move(*res);
    break;
  }
  default:
    return std::unexpected("not yet implemented");
  }

  switch (dest) {
  case Format::WAV: {
    auto res = codec::wav::encode(audio);
    if (!res)
      return std::unexpected(res.error());

    codec::wav::WavAudio wav_audio = std::move(*res);

    std::ofstream out(out_path, std::ios::binary);
    out.write(reinterpret_cast<const char *>(&wav_audio.master_header),
              sizeof(codec::wav::MasterRiffHeader));
    out.write(reinterpret_cast<const char *>(&wav_audio.fmt_chunk_header),
              sizeof(codec::wav::ChunkHeader));
    out.write(reinterpret_cast<const char *>(&wav_audio.fmt_chunk),
              sizeof(codec::wav::FmtChunk));
    out.write(reinterpret_cast<const char *>(&wav_audio.data_chunk_header),
              sizeof(codec::wav::ChunkHeader));
    out.write(reinterpret_cast<const char *>(wav_audio.pcm.data()),
              wav_audio.pcm.size());

    break;
  }
  default:
    return std::unexpected("not yet implemented");
  }

  return {};
}

} // namespace prisma
