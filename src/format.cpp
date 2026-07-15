#include <algorithm>
#include <prisma/format.hpp>

namespace prisma {

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
