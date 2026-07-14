#pragma once

#include <cstdint>
#include <expected>
#include <span>
#include <string>

namespace prisma {

std::expected<void, std::string>print_bmp_info(std::span<const uint8_t> file_data);

}
