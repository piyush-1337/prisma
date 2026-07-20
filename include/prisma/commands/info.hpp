#pragma once

#include <cstdint>
#include <expected>
#include <span>
#include <string>

namespace prisma {

namespace codec::bmp {

std::expected<void, std::string> print_info(std::span<const uint8_t> file_data);

}

namespace codec::png {

std::expected<void, std::string> print_info(std::span<const uint8_t> file_data);

}

namespace codec::wav {

std::expected<void, std::string> print_info(std::span<const uint8_t> file_data);

}


} // namespace prisma
