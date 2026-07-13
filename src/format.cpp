#include <algorithm>
#include <prisma/format.hpp>

namespace prisma {

constexpr std::array<uint8_t, 2> MAGIC_BMP = {0x42, 0x4D};
constexpr std::array<uint8_t, 4> MAGIC_WAV = {0x52, 0x49, 0x46, 0x46};
constexpr std::array<uint8_t, 4> MAGIC_FLAC = {0x66, 0x4C, 0x61, 0x43};
constexpr std::array<uint8_t, 8> MAGIC_PNG = {0x89, 0x50, 0x4E, 0x47,
                                              0x0D, 0x0A, 0x1A, 0x0A};

Format identify_format(std::span<const uint8_t> file_data) {
  auto starts_with = [&](const auto &magic) {
    return file_data.size() >= magic.size() &&
           std::ranges::equal(file_data.subspan(0, magic.size()), magic);
  };

  if (starts_with(MAGIC_BMP)) {
    return Format::BMP;
  }
  if (starts_with(MAGIC_WAV)) {
    return Format::WAV;
  }
  if (starts_with(MAGIC_FLAC)) {
    return Format::FLAC;
  }
  if (starts_with(MAGIC_PNG)) {
    return Format::PNG;
  }

  return Format::UNKNOWN;
}

} // namespace prisma
