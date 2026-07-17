#pragma once

#include <cstdint>
#include <expected>
#include <span>
#include <string>

namespace prisma {

namespace format {

namespace bmp {

std::expected<void, std::string>
print_info(std::span<const uint8_t> file_data);

}

namespace png {

std::expected<void, std::string>
print_info(std::span<const uint8_t> file_data);

}

} // namespace format

} // namespace prisma
