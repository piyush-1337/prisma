#pragma once

#include <cstdint>
#include <filesystem>
#include <span>

namespace prisma {

enum class MediaType { IMAGE, AUDIO, UNKNOWN };
enum class Format { BMP, WAV, FLAC, PNG, UNKNOWN };

constexpr std::array<uint8_t, 2> MAGIC_BMP = {0x42, 0x4D};
constexpr std::array<uint8_t, 4> MAGIC_WAV = {0x52, 0x49, 0x46, 0x46};
constexpr std::array<uint8_t, 4> MAGIC_FLAC = {0x66, 0x4C, 0x61, 0x43};
constexpr std::array<uint8_t, 8> MAGIC_PNG = {0x89, 0x50, 0x4E, 0x47,
                                              0x0D, 0x0A, 0x1A, 0x0A};

Format identify_format(std::span<const uint8_t> file_data);
Format identify_format_from_ext(const std::filesystem::path &path);

inline MediaType get_media_type(Format format) {
  if (format == Format::BMP || format == Format::PNG)
    return MediaType::IMAGE;
  if (format == Format::WAV || format == Format::FLAC)
    return MediaType::AUDIO;

  return MediaType::UNKNOWN;
}

} // namespace prisma
