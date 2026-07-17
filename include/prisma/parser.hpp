#pragma once

#include <cstdint>
#include <expected>
#include <prisma/formats/bmp/bmp.hpp>
#include <prisma/formats/png/png.hpp>
#include <span>
#include <string>

namespace prisma {

namespace format::bmp {

std::expected<std::pair<BmpFileHeader, BmpInfoHeader>, std::string>
parse_header(std::span<const uint8_t> file_data);

}

namespace format::png {

std::expected<PngImageHeader, std::string>
parse_header(std::span<const uint8_t> file_data);

}

} // namespace prisma
