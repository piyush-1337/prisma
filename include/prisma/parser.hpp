#pragma once

#include <cstdint>
#include <expected>
#include <prisma/formats/bmp.hpp>
#include <span>
#include <string>

namespace prisma {

std::expected<std::pair<BmpFileHeader, BmpInfoHeader>, std::string> parse_bmp(std::span<const uint8_t> file_data);

}
