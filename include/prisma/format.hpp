#include <cstdint>
#include <span>

namespace prisma {

enum class Format { BMP, WAV, FLAC, PNG, UNKNOWN };

Format identify_format(std::span<const uint8_t> file_data);

} // namespace prisma
