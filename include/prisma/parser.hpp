#pragma once

#include <cstdint>
#include <expected>
#include <prisma/formats/bmp.hpp>
#include <prisma/formats/png.hpp>
#include <span>
#include <string>

namespace prisma {

std::expected<std::pair<BmpFileHeader, BmpInfoHeader>, std::string> parse_bmp(std::span<const uint8_t> file_data);
std::expected<PngImageHeader, std::string> parse_png(std::span<const uint8_t> file_data);

}
